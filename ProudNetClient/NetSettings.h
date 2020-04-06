#pragma once
#include <Windows.h>
#include <cstdint>

namespace Nettention
{
    namespace Proud
    {
        class NetSettings
        {
        public:
            bool AllowServerAsP2PGroupMember;
            byte DirectP2PStartCondition;
            uint32_t EmergencyLogLineCount;
            bool EnableLookaheadP2PSend;
            bool EnableNagleAlgorithm;
            bool EnableP2PEncryptedMessaging;
            bool EnablePingTest;
            bool EnableServerLog;
            uint32_t EncryptedMessageKeyLength;
            byte FallbackMethod;
            uint32_t FastEncryptedMessageKeyLength;
            double IdleTimeout;
            uint32_t MessageMaxLength;
            uint32_t OverSendSuspectingThresholdInBytes;
            bool UpnpDetectNatDevice;
            bool UpnpTcpAddrPortMapping;
        };
    }
}