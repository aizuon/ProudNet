#pragma once
#include <WS2tcpip.h>
#include <Windows.h>
#include <cstdint>
#include <mutex>
#include <atomic>

#include "../BaseLib/IPEndPoint.h"
#include "NetMessage.h"

namespace Nettention
{
    namespace Proud
    {
        class NetClient;
        class P2PMember
        {
            friend class NetHandler;

        public:
            uint32_t GroupId = 0;
            uint32_t HostId = 0;

            P2PMember(NetClient* owner, uint32_t groupId, uint32_t hostId);

            BaseLib::IPEndPoint PeerUdpSocket;
            BaseLib::IPEndPoint PeerUdpLocalSocket;
            BaseLib::IPEndPoint SelfUdpSocket;
            BaseLib::IPEndPoint SelfUdpLocalSocket;

            BaseLib::IPEndPoint PeerLocalToRemoteSocket;
            BaseLib::IPEndPoint PeerRemoteToLocalSocket;
            BaseLib::IPEndPoint SelfLocalToRemoteSocket;
            BaseLib::IPEndPoint SelfRemoteToLocalSocket;

            std::atomic<bool> DirectP2P = false;

            GUID PeerUdpMagicNumber;

            void ConnectUdp();

            void RmiToPeer(uint16_t rmiID, NetMessage& message, bool forceRelay = false);
            void ProudToPeer(NetMessage& data, BaseLib::IPEndPoint udpEndPoint = BaseLib::IPEndPoint::Empty);

            void Close();

        private:
            NetClient* owner;

            SOCKET PeerUdpChannel = INVALID_SOCKET;
            std::thread ListenUdpThread;

            std::atomic<bool> UdpConnected = false;

            std::mutex mtx;

            uint32_t _selfFrameNumber = 0;

            void ToPeer(NetMessage& message, BaseLib::IPEndPoint udpEndPoint = BaseLib::IPEndPoint::Empty);
            void ToPeer(byte* data, size_t size, BaseLib::IPEndPoint udpEndPoint = BaseLib::IPEndPoint::Empty);

            void ListenUdp();
        };
    }
}