#pragma once
#include <WS2tcpip.h>
#include <Windows.h>
#include <cstdint>
#include <openssl/rsa.h>
#include <concurrent_unordered_map.h>
#include <string>
#include <thread>
#include <atomic>
#include <functional>
#include <mutex>
#include <tuple>

#include "../BaseLib/Logger.h"
#include "Enums.h"
#include "NetCrypt.h"
#include "NetSettings.h"
#include "P2PGroup.h"
#include "NetMessage.h"
#include "../BaseLib/IPEndPoint.h"
#include "../BaseLib/ThreadLoop.h"

namespace Nettention
{
    namespace Proud
    {
        class NetClient
        {
            friend class NetHandler;
            friend class P2PMember;

        public:
            BaseLib::Logger m_Logger;

            ServerType m_ServerType;
            uint32_t HostId = 0;
            P2PGroup m_P2PGroup;

            static concurrency::concurrent_unordered_map<ServerType, NetClient*> Clients;

            std::function<void()> OnConnectionComplete = []() {};
            std::function<void(NetMessage&, uint16_t)> OnRMIRecieve = [](NetMessage& message, uint16_t rmiID) {};

            GUID ServerGuid;
            GUID UdpMagicNumber;
            GUID PeerUdpMagicNumber;
            BaseLib::IPEndPoint ServerEndPoint;

            NetClient(ServerType serverType);

            ~NetClient();

            void Connect(const BaseLib::IPEndPoint& endPoint, bool nonBlocking = false);
            void ConnectUdp();
            bool IsConnected();
			bool IsUdpConnected();
            void Disconnect();

            void Close();
            void CloseUdp();

            void RmiToServer(uint16_t rmiID, NetMessage& message, EncryptMode ecMode = EncryptMode::Secure);
            void ProudToServer(NetMessage& data);
            void RmiToServerUdpIfAvaliable(uint16_t rmiID, NetMessage& message, EncryptMode ecMode = EncryptMode::Secure);
            void ProudToServerUdpIfAvaliable(NetMessage& data, bool force = false);

        protected:
            NetCrypt m_NetCrypt;
            RSA* m_RSA = nullptr;
            NetSettings m_NetSettings;

        private:
            static std::atomic<bool> wsInit;

            SOCKET Channel = INVALID_SOCKET;
            std::thread ListenThread;

            std::atomic<bool> Connected = false;

            SOCKET UdpChannel = INVALID_SOCKET;
            std::thread ListenUdpThread;
            BaseLib::IPEndPoint ServerUdpSocket;
            BaseLib::IPEndPoint SelfUdpSocket;

            std::atomic<bool> UdpEnabled = false;
            std::atomic<bool> UdpConnected = false;

            BaseLib::ThreadLoop<NetClient> PingLoop;
            void SendUdpPing();

            void Listen();
            void ListenUdp();

            const static uint32_t MessageMaxLength = 1048576;

            void ToServer(NetMessage& message);
            void ToServer(byte* data, size_t size, bool retry = false);
            void ToServerUdp(NetMessage& message);
            void ToServerUdp(byte* data, size_t size);

            std::atomic<bool> NonBlocking = false;
        };
    }
}