#pragma once
#include "NetClient.h"
#include "NetMessage.h"
#include "../BaseLib/IPEndPoint.h"

namespace Nettention
{
    namespace Proud
    {
        class NetHandler
        {
        public:
            static void SplitAndProcessStream(NetClient& client, byte* buffer, size_t length, const BaseLib::IPEndPoint& udpEndPoint = BaseLib::IPEndPoint::Empty);
            static void ReadFrame(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint = BaseLib::IPEndPoint::Empty);

        private:
            static void ReadMessage(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint = BaseLib::IPEndPoint::Empty);

            static void RMIHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint);
            static void NotifyServerConnectionHintHandler(NetClient& client, NetMessage& message);
            static void NotifyCSSessionKeySuccessHandler(NetClient& client);
            static void NotifyServerConnectSuccessHandler(NetClient& client, NetMessage& message);
            static void UnreliableRelay2Handler(NetClient& client, NetMessage& message);
            static void ReliableRelay2Handler(NetClient& client, NetMessage& message);
            static void EncryptedHandler(NetClient& client, NetMessage& message, bool reliable = true);
            static void CompressedHandler(NetClient& client, NetMessage& message);
            static void RequestStartServerHolepunchHandler(NetClient& client, NetMessage& message);
            static void ServerHolepunchAckHandler(NetClient& client, NetMessage& message);
            static void NotifyClientServerUdpMatchedHandler(NetClient& client, NetMessage& message);
            static void PeerUdpServerHolepunchAckHandler(NetClient& client, NetMessage& message);
            static void PeerUdpPeerHolepunchHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint);
            static void PeerUdpPeerHolepunchAckHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint);
            static void P2PRequestIndirectServerTimeAndPingHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint);
            static void P2PReplyIndirectServerTimeAndPongHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint);
            static void ReliableUdpFrameHandler(NetClient& client, NetMessage& message, const BaseLib::IPEndPoint& udpEndPoint);
        };
    }
}