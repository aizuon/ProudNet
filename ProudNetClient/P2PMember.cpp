#include <fmt/format.h>

#include "P2PMember.h"
#include "NetClient.h"
#include "Constants.h"
#include "NetHandler.h"
#include "FilterTag.h"

namespace Nettention
{
    namespace Proud
    {
        P2PMember::P2PMember(NetClient* owner, uint32_t groupId, uint32_t hostId)
        {
            this->owner = owner;
            GroupId = groupId;
            HostId = hostId;
        }

        void P2PMember::ConnectUdp()
        {
            if (!owner->wsInit)
            {
                WSADATA wsData;
                WORD ver = MAKEWORD(2, 2);

                int wsOk = WSAStartup(ver, &wsData);
                if (wsOk != 0)
                {
                    return;
                }
                owner->wsInit = true;
            }

            SOCKET channel = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (channel == INVALID_SOCKET)
            {
                return;
            }

            int sendBuff = 51200;
            int sendBuffResult = setsockopt(channel, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuff, sizeof(int));
            int recvBuff = 51200;
            int recvBuffResult = setsockopt(channel, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuff, sizeof(int));
            if (sendBuffResult == SOCKET_ERROR || recvBuffResult == SOCKET_ERROR)
            {
                closesocket(channel);
                return;
            }

            sockaddr_in add;
            add.sin_family = AF_INET;
            add.sin_addr.s_addr = htonl(INADDR_ANY);
            add.sin_port = htons(0);

            int ret = bind(channel, (const sockaddr*)&add, sizeof(sockaddr_in));
            if (ret == SOCKET_ERROR)
            {
                ret = WSAGetLastError();
                if (ret != WSAEWOULDBLOCK)
                {
                    closesocket(channel);
                    return;
                }
            }

            PeerUdpChannel = channel;

            UdpConnected = true;
            ListenUdpThread = std::thread(&P2PMember::ListenUdp, this);
            ListenUdpThread.detach();
        }

        void P2PMember::RmiToPeer(uint16_t rmiID, NetMessage& message, bool forceRelay)
        {
            NetMessage data;

            data.WriteEnum(MessageType::RMI);
            data.ByteArray::Write(rmiID);
            data.Write(message);

            if (DirectP2P && !forceRelay)
            {
                if (message.Reliable)
                {
                    NetMessage udpframe;
                    udpframe.WriteEnum(MessageType::ReliableUdp_Frame);
                    udpframe.ByteArray::Write((byte)1);
                    udpframe.ByteArray::Write(_selfFrameNumber);
                    _selfFrameNumber++;

                    NetMessage test;
                    test.ByteArray::Write(Constants::Magic);
                    test.ByteArray::Write((ByteArray)data);

                    udpframe.Write((ByteArray)test);

                    ProudToPeer(udpframe);
                }
                else
                {
                    ProudToPeer(data);
                }

                return;
            }

            if (message.Reliable)
            {
                NetMessage reliablerelay;
                reliablerelay.WriteEnum(MessageType::ReliableRelay1);
                reliablerelay.WriteScalar(1);
                reliablerelay.ByteArray::Write(HostId);
                reliablerelay.ByteArray::Write(_selfFrameNumber);
                _selfFrameNumber++;

                NetMessage test;
                test.ByteArray::Write(Constants::Magic);
                test.Write((ByteArray)data);

                reliablerelay.Write((ByteArray)test);

                owner->ProudToServerUdpIfAvaliable(reliablerelay);
            }
            else
            {
                NetMessage unreliablerelay;
                unreliablerelay.WriteEnum(MessageType::UnreliableRelay1);
                unreliablerelay.WriteEnum(MessagePriority::Ring0);
                unreliablerelay.WriteScalar(0); //unq id
                unreliablerelay.WriteScalar(1);
                unreliablerelay.ByteArray::Write(HostId);
                unreliablerelay.Write((ByteArray)data);

                owner->ProudToServerUdpIfAvaliable(unreliablerelay);
            }
        }

        void P2PMember::ProudToPeer(NetMessage& data, BaseLib::IPEndPoint udpEndPoint)
        {
            ToPeer(data, udpEndPoint);
        }

        void P2PMember::ToPeer(NetMessage& message, BaseLib::IPEndPoint udpEndPoint)
        {
            ToPeer(message.GetBuffer(), message.GetLength(), udpEndPoint);
        }

        void P2PMember::ToPeer(byte* data, size_t size, BaseLib::IPEndPoint udpEndPoint)
        {
            BaseLib::IPEndPoint& dest = PeerLocalToRemoteSocket.IsEmpty() ? udpEndPoint : PeerLocalToRemoteSocket;

            if (dest.IsEmpty())
                return;

            sockaddr_in add;
            add.sin_family = AF_INET;
            inet_pton(AF_INET, dest.IP.c_str(), &add.sin_addr.s_addr);
            add.sin_port = htons(dest.Port);

            NetMessage message;
            message.ByteArray::Write(Constants::Magic);
            ByteArray arr(data, size);
            message.ByteArray::Write(arr);

            NetMessage header;
            header.ByteArray::Write((uint16_t)43981);
            header.ByteArray::Write(FilterTag::CreateFilterTag(owner->HostId, HostId));
            header.ByteArray::Write((uint32_t)message.GetLength());
            header.ByteArray::Write((uint32_t)0);
            header.ByteArray::Write((uint32_t)0);

            NetMessage udpmessage;
            udpmessage.Write(header);
            udpmessage.Write(message);

            int SendBytes = sendto(PeerUdpChannel, (const char*)udpmessage.GetBuffer(), udpmessage.GetLength(), 0, (const sockaddr*)&add, sizeof(sockaddr_in));
            if (SendBytes == SOCKET_ERROR)
            {
                int err = WSAGetLastError();
                if (err != WSAEWOULDBLOCK)
                {
                    UdpConnected = false;
                    return;
                }
                else
                {
                    int result = 0;

                    while (result != 1)
                    {
                        static const timeval sixtyFPS = { 0, 16670 };
                        fd_set a = { 1, { PeerUdpChannel } };

                        result = select(0, 0, &a, 0, &sixtyFPS);

                        if (result == SOCKET_ERROR)
                        {
                            result = WSAGetLastError();
                            if (result != WSAEWOULDBLOCK)
                            {
                                UdpConnected = false;
                                return;
                            }
                        }
                    }
                }
            }
        }

        void P2PMember::ListenUdp()
        {
            byte* buffer = new byte[owner->MessageMaxLength];
            while (UdpConnected)
            {
                ZeroMemory(buffer, owner->MessageMaxLength);
                sockaddr_in sender;
                socklen_t sendsize = sizeof(sockaddr_in);
                int RecvBytes = recvfrom(PeerUdpChannel, (char*)buffer, owner->MessageMaxLength, 0, (sockaddr*)&sender, &sendsize);
                if (RecvBytes > 0)
                {
                    NetMessage data(buffer, RecvBytes);
                    uint16_t flag = 0;
                    uint16_t sessionid = 0;
                    int32_t length = 0;
                    uint32_t id = 0;
                    uint32_t fragid = 0;
                    if (!data.ByteArray::Read(flag) || !data.ByteArray::Read(sessionid) || !data.ByteArray::Read(length) || !data.ByteArray::Read(id) || !data.ByteArray::Read(fragid))
                        continue;
                    char str[INET_ADDRSTRLEN];
                    NetHandler::ReadFrame(*owner, data, BaseLib::IPEndPoint(inet_ntop(AF_INET, &sender.sin_addr, str, INET_ADDRSTRLEN), ntohs(sender.sin_port)));
                }
                else if (RecvBytes == 0)
                {
                    UdpConnected = false;
                    break;
                }
                else if (RecvBytes == SOCKET_ERROR)
                {
                    int err = WSAGetLastError();
                    if (err != WSAEWOULDBLOCK)
                    {
                        UdpConnected = false;
                        break;
                    }
                    else
                    {
                        int result = 0;

                        while (result != 1)
                        {
                            static const timeval sixtyFPS = { 0, 16670 };
                            fd_set a = { 1, { PeerUdpChannel } };

                            result = select(0, &a, 0, 0, &sixtyFPS);

                            if (result == SOCKET_ERROR)
                            {
                                result = WSAGetLastError();
                                if (result != WSAEWOULDBLOCK)
                                {
                                    UdpConnected = false;
                                    break;
                                }
                            }
                        }
                    }
                }
                if (!owner->NonBlocking)
                    std::this_thread::sleep_for(std::chrono::microseconds(16670));
            }
            delete[] buffer;
        }

        void Nettention::Proud::P2PMember::Close()
        {
            UdpConnected = false;
            DirectP2P = false;

            if (PeerUdpChannel != INVALID_SOCKET)
            {
                closesocket(PeerUdpChannel);
                PeerUdpChannel = INVALID_SOCKET;
            }
        }
    }
}