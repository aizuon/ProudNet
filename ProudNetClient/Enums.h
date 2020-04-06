#pragma once
#include <Windows.h>

namespace Nettention
{
    namespace Proud
    {
        enum class ServerType : byte
        {
            Auth,
            Game,
            Chat,
            Relay
        };

        enum class EncryptMode : byte
        {
            None,
            Secure,
            Fast
        };

        enum class MessageType : byte
        {
            None = 0,
            RMI = 1,
            UserMessage = 2,
            ConnectServerTimedout = 3,
            NotifyServerConnectionHint = 4,
            NotifyCSEncryptedSessionKey = 5,
            NotifyCSSessionKeySuccess = 6,
            NotifyServerConnectionRequestData = 7,
            NotifyProtocolVersionMismatch = 8,
            NotifyServerDeniedConnection = 9,
            NotifyServerConnectSuccess = 10,
            RequestStartServerHolepunch = 11,
            ServerHolepunch = 12,
            ServerHolepunchAck = 13,
            NotifyHolepunchSuccess = 14,
            NotifyClientServerUdpMatched = 15,
            PeerUdp_ServerHolepunch = 16,
            PeerUdp_ServerHolepunchAck = 17,
            PeerUdp_NotifyHolepunchSuccess = 18,
            ReliableUdp_Frame = 19,
            ReliableRelay1 = 20,
            UnreliableRelay1 = 21,
            UnreliableRelay1_RelayDestListCompressed = 22,
            ReliableRelay2 = 23,
            UnreliableRelay2 = 24,
            LingerDataFrame2 = 25,
            UnreliablePing = 26,
            SpeedHackDetectorPing = 27,
            UnreliablePong = 28,
            ArbitaryTouch = 29,
            PeerUdp_PeerHolepunch = 30,
            PeerUdp_PeerHolepunchAck = 31,
            P2PRequestIndirectServerTimeAndPing = 32,
            P2PReplyIndirectServerTimeAndPong = 33,
            S2CRoutedMulticast1 = 34,
            S2CRoutedMulticast2 = 35,
            Encrypted_Reliable = 36,
            Encrypted_UnReliable = 37,
            Compressed = 38,
            RequestReceiveSpeedAtReceiverSide_NoRelay = 39,
            ReplyReceiveSpeedAtReceiverSide_NoRelay = 40,
            NotifyConnectionPeerRequestData = 41,
            NotifyCSP2PDisconnected = 42,
            NotifyConnectPeerRequestDataSucess = 43,
            NotifyCSConnectionPeerSuccess = 44,
            Ignore = 45,
            RequestServerConnectionHint = 46,
            PolicyRequest = 47,
            P2PReliablePing = 48,
            P2PReliablePong = 49,
            INTERN_ENCRYPTED_AES_RELIABLE_COMPRESSED = 100,
            INTERN_ENCRYPTED_AES_UNRELIABLE_COMPRESSED = 101,
            INTERN_ENCRYPTED_RC4_RELIABLE_COMPRESSED = 102,
            INTERN_ENCRYPTED_RC4_UNRELIABLE_COMPRESSED = 103
        };

        enum class MessagePriority : byte
        {
            Ring0,
            Ring1,
            High,
            Medium,
            Low,
            Ring99,
            Last
        };

        enum class ProudOpCode : uint16_t
        {
            // C2S
            ReliablePing = 64001,
            P2P_NotifyDirectP2PDisconnected = 64002,
            NotifyUdpToTcpFallbackByClient = 64003,
            P2PGroup_MemberJoin_Ack = 64004,
            NotifyP2PHolepunchSuccess = 64005,
            ShutdownTcp = 64006,
            ShutdownTcpHandshake = 64007,
            NotifyLog = 64008,
            NotifyLogHolepunchFreqFail = 64009,
            NotifyNatDeviceName = 64010,
            NotifyPeerUdpSocketRestored = 64011,
            NotifyJitDirectP2PTriggered = 64012,
            NotifyNatDeviceNameDetected = 64013,
            NotifySendSpeed = 64014,
            ReportP2PPeerPing = 64015,
            C2S_RequestCreateUdpSocket = 64016,
            C2S_CreateUdpSocketAck = 64017,
            ReportC2CUdpMessageCount = 64018,
            ReportC2SUdpMessageTrialCount = 64019,

            // S2C
            P2PGroup_MemberJoin = 64501,
            P2PGroup_MemberJoin_Unencrypted = 64502,
            P2PRecycleComplete = 64503,
            RequestP2PHolepunch = 64504,
            P2P_NotifyDirectP2PDisconnected2 = 64505,
            P2PGroup_MemberLeave = 64506,
            NotifyDirectP2PEstablish = 64507,
            ReliablePong = 64508,
            EnableLog = 64509,
            DisableLog = 64510,
            NotifyUdpToTcpFallbackByServer = 64511,
            NotifySpeedHackDetectorEnabled = 64512,
            ShutdownTcpAck = 64513,
            RequestAutoPrune = 64514,
            RenewP2PConnectionState = 64515,
            NewDirectP2PConnection = 64516,
            RequestMeasureSendSpeed = 64517,
            S2C_RequestCreateUdpSocket = 64518,
            S2C_CreateUdpSocketAck = 64519,

            // C2C
            Rmi_First = 65000,
            HolsterP2PHolepunchTrial = 65001,
            ReportUdpMessageCount = 65002,
            ReportServerTimeAndFrameRateAndPing = 65003,
            ReportServerTimeAndFrameRateAndPong = 65004,
            Rmi_Last = 65005
        };

        enum class LocalEventType
        {
            None,
            ConnectServerSuccess,
            ConnectServerFail,
            ClientServerDisconnect,
            ClientJoinCandidate,
            ClientJoinApproved,
            ClientLeaveAfterDispose,
            AddMemberAckComplete,
            AddMember,
            DelMember,
            DirectP2PEnabled,
            RelayP2PEnabled,
            GroupP2PEnabled,
            ServerUdpChanged,
            SynchronizeServerTime,
            HackSuspected,
            TcpListenFail,
            P2PGroupRemoved,
            P2PDisconnected,
            UnitTestFail,
            Error,
            Warning
        };

        enum class ErrorType
        {
            Ok,
            Unexpected,
            AlreadyConnected,
            TCPConnectFailure,
            InvalidSessionKey,
            EncryptFail,
            DecryptFail,
            ConnectServerTimeout,
            ProtocolVersionMismatch,
            NotifyServerDeniedConnection,
            ConnectServerSuccessful,
            DisconnectFromRemote,
            DisconnectFromLocal,
            DangerousArgumentWarning,
            UnknownAddrPort,
            ServerNotReady,
            ServerPortListenFailure,
            AlreadyExists,
            PermissionDenied,
            BadSessionGuid,
            InvalidCredential,
            InvalidHeroName,
            LoadDataPreceded,
            AdjustedGamerIDNotFilled,
            NoHero,
            UnitTestFailed,
            P2PUdpFailed,
            ReliableUdpFailed,
            ServerUdpFailed,
            NoP2PGroupRelation,
            UserRequested,
            InvalidPacketFormat,
            TooLargeMessageDetected,
            CannotEncryptUnreliableMessage,
            ValueNotExist,
            TimeOut,
            LoadedDataNotFound,
            SendQueueIsHeavy,
            TooSlowHeartbeatWarning,
            CompressFail,
            LocalSocketCreationFailed,
            NoneAvailableInPortPool,
            InvalidPortPool
        };
    }
}