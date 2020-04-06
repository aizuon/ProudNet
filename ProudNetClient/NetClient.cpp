#include <fmt/format.h>
#include <vector>
#include <chrono>
#include <exception>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib, "crypt32")

#include "NetClient.h"
#include "../BaseLib/ByteArrayUtils.h"
#include "NetHandler.h"
#include "NetZip.h"
#include "Constants.h"
#include "FilterTag.h"

namespace Nettention
{
    namespace Proud
    {
        static std::vector<std::string> LoggerNames = { "Auth", "Game", "Chat", "Relay" };

        concurrency::concurrent_unordered_map<ServerType, NetClient*> NetClient::Clients;

        std::atomic<bool> NetClient::wsInit = false;

        NetClient::NetClient(ServerType serverType)
            : m_Logger(LoggerNames[(int)serverType]), m_NetCrypt(0, 0)
        {
            m_ServerType = serverType;

            Clients.insert(std::make_pair(m_ServerType, this));
        }

        NetClient::~NetClient()
        {
            Connected = false;
            if (Channel != INVALID_SOCKET)
            {
                closesocket(Channel);
                Channel = INVALID_SOCKET;
            }

            UdpConnected = false;
            UdpEnabled = false;
            for (auto& x : m_P2PGroup.P2PMembers)
            {
                P2PMember* p2pmember = x.second;
                p2pmember->Close();
                delete p2pmember;
            }
            m_P2PGroup.P2PMembers.clear();
            if (UdpChannel != INVALID_SOCKET)
            {
                closesocket(UdpChannel);
                UdpChannel = INVALID_SOCKET;
            }

            if (m_RSA != nullptr)
            {
                RSA_free(m_RSA);
                m_RSA = nullptr;
            }

            Clients.unsafe_erase(m_ServerType);
            if (Clients.size() == 0)
            {
                wsInit = false;
                WSACleanup();
            }
        }

        void NetClient::Connect(const BaseLib::IPEndPoint& endPoint, bool nonBlocking)
        {
            ServerEndPoint = endPoint;
            NonBlocking = nonBlocking;

            if (!wsInit)
            {
                WSADATA wsData;
                WORD ver = MAKEWORD(2, 2);

                int wsOk = WSAStartup(ver, &wsData);
                if (wsOk != 0)
                {
                    m_Logger.Error("WS startup failed!");
                    return;
                }
                wsInit = true;
            }

            SOCKET channel = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (channel == INVALID_SOCKET)
            {
                m_Logger.Error("Create socket failed!");
                return;
            }

            int nagle = 0;
            int nagleResult = setsockopt(channel, SOL_SOCKET, TCP_NODELAY, (char*)&nagle, sizeof(int));
            int sendBuff = 51200;
            int sendBuffResult = setsockopt(channel, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuff, sizeof(int));
            int recvBuff = 51200;
            int recvBuffResult = setsockopt(channel, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuff, sizeof(int));
            int keepAlive = 0;
            int keepAliveResult = setsockopt(channel, SOL_SOCKET, SO_KEEPALIVE, (char*)&keepAlive, sizeof(int));
            if (nagleResult == SOCKET_ERROR || sendBuffResult == SOCKET_ERROR || recvBuffResult == SOCKET_ERROR || keepAliveResult == SOCKET_ERROR)
            {
                m_Logger.Error("Setting socket options failed!");
                closesocket(channel);
                return;
            }

            sockaddr_in hint;
            hint.sin_family = AF_INET;
            inet_pton(AF_INET, ServerEndPoint.IP.c_str(), &hint.sin_addr);
            hint.sin_port = htons(ServerEndPoint.Port);

            int connResult = connect(channel, (const sockaddr*)&hint, sizeof(sockaddr_in));
            if (connResult == SOCKET_ERROR)
            {
                connResult = WSAGetLastError();
                if (connResult != WSAEWOULDBLOCK)
                {
                    m_Logger.Error(fmt::format("Connection to {0}:{1} failed!", ServerEndPoint.IP, ServerEndPoint.Port));
                    closesocket(channel);
                    return;
                }
            }

            u_long mode = NonBlocking ? 1 : 0;
            int nonblockingResult = ioctlsocket(channel, FIONBIO, &mode);
            if (nonblockingResult != NO_ERROR)
            {
                m_Logger.Error("Setting socket non-blocking option failed!");
                closesocket(channel);
                return;
            }

            m_Logger.Information(fmt::format("Connection to {0}:{1} successful!", ServerEndPoint.IP, ServerEndPoint.Port));

            Channel = channel;

            Connected = true;
            ListenThread = std::thread(&NetClient::Listen, this);
            ListenThread.detach();
        }

        void NetClient::ConnectUdp()
        {
            if (!wsInit)
            {
                WSADATA wsData;
                WORD ver = MAKEWORD(2, 2);

                int wsOk = WSAStartup(ver, &wsData);
                if (wsOk != 0)
                {
                    m_Logger.Error("WS startup failed!");
                    return;
                }
                wsInit = true;
            }

            SOCKET channel = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            if (channel == INVALID_SOCKET)
            {
                m_Logger.Error("Create UDP socket failed!");
                return;
            }

            int sendBuff = 51200;
            int sendBuffResult = setsockopt(channel, SOL_SOCKET, SO_SNDBUF, (char*)&sendBuff, sizeof(int));
            int recvBuff = 51200;
            int recvBuffResult = setsockopt(channel, SOL_SOCKET, SO_RCVBUF, (char*)&recvBuff, sizeof(int));
            if (sendBuffResult == SOCKET_ERROR || recvBuffResult == SOCKET_ERROR)
            {
                m_Logger.Error("Setting UDP socket options failed!");
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
                    m_Logger.Error("Binding UDP socket failed!");
                    closesocket(channel);
                    return;
                }
            }

            UdpChannel = channel;
            
            UdpConnected = true;
            ListenUdpThread = std::thread(&NetClient::ListenUdp, this);
            ListenUdpThread.detach();
        }

        bool NetClient::IsConnected()
        {
            return Connected;
        }

        bool NetClient::IsUdpConnected()
        {
            return UdpConnected;
        }

        void NetClient::Disconnect()
        {
            NetClient::~NetClient();

            m_NetCrypt.~NetCrypt();
        }

        void NetClient::Close()
        {
            NetMessage shutdowntcp;

            shutdowntcp.ByteArray::Write((uint16_t)0);

            RmiToServer((uint16_t)ProudOpCode::ShutdownTcp, shutdowntcp);
        }

        void NetClient::CloseUdp()
        {
            UdpConnected = false;
            UdpEnabled = false;

            for (auto& x : m_P2PGroup.P2PMembers)
            {
                P2PMember* p2pmember = x.second;
                p2pmember->Close();
            }

            if (UdpChannel != INVALID_SOCKET)
            {
                closesocket(UdpChannel);
                UdpChannel = INVALID_SOCKET;
            }
        }

        void NetClient::SendUdpPing()
        {
            NetMessage unreliableping;
            unreliableping.WriteEnum(MessageType::UnreliablePing);
            unreliableping.ByteArray::Write(0.00);
            unreliableping.ByteArray::Write(0.05); //50ms

            ProudToServerUdpIfAvaliable(unreliableping, true);
        }

        void NetClient::Listen()
        {
            byte* buffer = new byte[MessageMaxLength];
            while (Connected)
            {
                ZeroMemory(buffer, MessageMaxLength);
                int RecvBytes = recv(Channel, (char*)buffer, MessageMaxLength, 0);
                if (RecvBytes > 0)
                {
                    m_Logger.Verbose(fmt::format("Recieved message | {0} => {1}", RecvBytes, BaseLib::Utils::ByteArrayUtils::ToHexString(buffer, RecvBytes)));

                    NetHandler::SplitAndProcessStream(*this, buffer, RecvBytes);
                }
                else if (RecvBytes == 0)
                {
                    Connected = false;
                    m_Logger.Information("Client disconnected!");
                    break;
                }
                else if (RecvBytes == SOCKET_ERROR)
                {
                    int err = WSAGetLastError();
                    if (err != WSAEWOULDBLOCK)
                    {
                        Connected = false;
                        m_Logger.Information("Client disconnected!");
                        break;
                    }
                    else
                    {
                        int result = 0;

                        while (result != 1)
                        {
                            static const timeval sixtyFPS = { 0, 16670 };
                            fd_set a = { 1, { Channel } };

                            result = select(0, &a, 0, 0, &sixtyFPS);

                            if (result == SOCKET_ERROR)
                            {
                                result = WSAGetLastError();
                                if (result != WSAEWOULDBLOCK)
                                {
                                    Connected = false;
                                    m_Logger.Information("Client disconnected!");
                                    break;
                                }
                            }
                        }
                    }
                }
                if (!NonBlocking)
                    std::this_thread::sleep_for(std::chrono::microseconds(16670));
            }
            delete[] buffer;
        }

        void NetClient::ListenUdp()
        {
            byte* buffer = new byte[MessageMaxLength];
            while (UdpConnected)
            {
                ZeroMemory(buffer, MessageMaxLength);
                sockaddr_in sender;
                socklen_t sendsize = sizeof(sockaddr_in);
                int RecvBytes = recvfrom(UdpChannel, (char*)buffer, MessageMaxLength, 0, (sockaddr*)&sender, &sendsize);
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
                    NetHandler::ReadFrame(*this, data, BaseLib::IPEndPoint(inet_ntop(AF_INET, &sender.sin_addr, str, INET_ADDRSTRLEN), ntohs(sender.sin_port)));
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
                            fd_set a = { 1, { UdpChannel } };

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
                if (!NonBlocking)
                    std::this_thread::sleep_for(std::chrono::microseconds(16670));
            }
            delete[] buffer;
        }

        void NetClient::RmiToServer(uint16_t rmiID, NetMessage& message, EncryptMode ecMode)
        {
            NetMessage data;
            if (ecMode != EncryptMode::None)
            {
                message.Encrypt = true;
                message.m_EncryptMode = EncryptMode::Secure;
            }

            data.WriteEnum(MessageType::RMI);
            data.ByteArray::Write(rmiID);
            data.Write(message);
            ProudToServer(data);
        }

        void NetClient::ProudToServer(NetMessage& data)
        {
            if (data.Compress)
                data = NetZip::CompressPacket(data);
            if (data.Encrypt)
            {
                if (data.m_EncryptMode == EncryptMode::None)
                {
                    data.m_EncryptMode = EncryptMode::Secure;
                    data.Reliable = true;
                }

                size_t encryptedSize = 0;
                bool encryptResult = false;
                byte* encrypted = nullptr;
                try
                {
                    encrypted = m_NetCrypt.Encrypt(data.GetBuffer(), data.GetLength(), encryptedSize, data.m_EncryptMode, encryptResult, data.Reliable);
                }
                catch (std::exception ex)
                {
                    m_Logger.Error(ex.what());
                    return;
                }
                if (!encryptResult)
                    return;
                NetMessage message;
                message.WriteEnum(data.Reliable ? MessageType::Encrypted_Reliable : MessageType::Encrypted_UnReliable);
                message.WriteEnum(data.m_EncryptMode);
                message.ByteArray::Write(ByteArray(encrypted, encryptedSize));
                delete[] encrypted;
                data = message;
            }

            NetMessage message2;
            message2.ByteArray::Write(Constants::Magic);
            message2.ByteArray::Write((ByteArray)data);
            ToServer(message2);
        }

        void NetClient::RmiToServerUdpIfAvaliable(uint16_t rmiID, NetMessage& message, EncryptMode ecMode)
        {
            NetMessage data;
            if (ecMode != EncryptMode::None)
            {
                message.Encrypt = true;
                message.m_EncryptMode = EncryptMode::Secure;
            }

            data.WriteEnum(MessageType::RMI);
            data.ByteArray::Write(rmiID);
            data.Write(message);
            if (UdpEnabled)
                ToServerUdp(data);
            else
                ProudToServer(data);
        }

        void NetClient::ProudToServerUdpIfAvaliable(NetMessage& data, bool force)
        {
            if (UdpEnabled || force)
                ToServerUdp(data);
            else
                ProudToServer(data);
        }

        void NetClient::ToServer(NetMessage& message)
        {
            ToServer(message.GetBuffer(), message.GetLength());
        }

        void NetClient::ToServer(byte* data, size_t size, bool retry)
        {
            if (Connected)
            {
                if (!retry)
                {
                    m_Logger.Verbose(fmt::format("Sending message | {0} => {1}", size, BaseLib::Utils::ByteArrayUtils::ToHexString(data, size)));
                }
                else
                {
                    m_Logger.Verbose(fmt::format("Retrying sending message | {0} => {1}", size, BaseLib::Utils::ByteArrayUtils::ToHexString(data, size)));
                }

                int SendBytes = send(Channel, (const char*)data, size, 0);
                if (SendBytes == SOCKET_ERROR)
                {
                    int err = WSAGetLastError();
                    if (err != WSAEWOULDBLOCK)
                    {
                        Connected = false;
                        m_Logger.Information("Client disconnected!");
                        return;
                    }
                    else
                    {
                        int result = 0;

                        while (result != 1)
                        {
                            static const timeval sixtyFPS = { 0, 16670 };
                            fd_set a = { 1, { Channel } };

                            result = select(0, 0, &a, 0, &sixtyFPS);

                            if (result == SOCKET_ERROR)
                            {
                                result = WSAGetLastError();
                                if (result != WSAEWOULDBLOCK)
                                {
                                    Connected = false;
                                    m_Logger.Information("Client disconnected!");
                                    return;
                                }
                            }
                        }

                        ToServer(data, size, true);
                    }
                }
            }
        }

        void NetClient::ToServerUdp(NetMessage& message)
        {
            ToServerUdp(message.GetBuffer(), message.GetLength());
        }

        void NetClient::ToServerUdp(byte* data, size_t size)
        {
            sockaddr_in add;
            add.sin_family = AF_INET;
            inet_pton(AF_INET, ServerUdpSocket.IP.c_str(), &add.sin_addr.s_addr);
            add.sin_port = htons(ServerUdpSocket.Port);

            NetMessage message;
            message.ByteArray::Write(Constants::Magic);
            ByteArray arr(data, size);
            message.ByteArray::Write(arr);

            NetMessage header;
            header.ByteArray::Write((uint16_t)43981);
            header.ByteArray::Write(FilterTag::CreateFilterTag(HostId, 1));
            header.ByteArray::Write((uint32_t)message.GetLength());
            header.ByteArray::Write((uint32_t)0);
            header.ByteArray::Write((uint32_t)0);

            NetMessage udpmessage;
            udpmessage.Write(header);
            udpmessage.Write(message);

            int SendBytes = sendto(UdpChannel, (const char*)udpmessage.GetBuffer(), udpmessage.GetLength(), 0, (const sockaddr*)&add, sizeof(sockaddr_in));
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
                        fd_set a = { 1, { UdpChannel } };

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
    }
}