#include "RelayClient.h" //cant be included below because of shitty windows header
#include "AuthClient.h" //cant be included below because of shitty windows header
#include "GameClient.h" //cant be included below because of shitty windows header
#include <string>
#include <fmt/format.h>

#include "RelayHandler.h"
#include "../BaseLib/StringUtils.h"
#include "S4RMI.h"
#include "../ProudNetClient/NetZip.h"
#include "Extensions.h"

using namespace Nettention::Proud;
using namespace BaseLib::Utils;

extern byte RoomId;
extern uint32_t ServerId;
extern uint32_t ChannelId;

RelayHandler::RelayHandler(Nettention::Proud::NetClient* client)
    : Handler(client)
{
}

void RelayHandler::InitHandler()
{
    NetMessage relayloginreq;
    relayloginreq.ByteArray::Write(AuthClient::GetInstance().m_ClientlessPlayer.AccountId);
    relayloginreq.Write(StringUtils::EmptyString);
    relayloginreq.ByteArray::Write(RoomId + 100000 * ServerId + 1000 * ChannelId);
    relayloginreq.ByteArray::Write(false);

    GetClient().RmiToServer((uint16_t)S10RMI::Relay_Login_Req, relayloginreq);
}

void RelayHandler::HandleRMI(Nettention::Proud::NetMessage& message, uint16_t rmiID)
{
    switch ((S10RMI)rmiID)
    {
    case S10RMI::Relay_Login_Ack:
    {
        GetClient().m_Logger.Information("Relay_Login_Ack");

        std::thread t = std::thread([]()
            {
                NetMessage roomenterplayerreq;
                GameClient::GetInstance().RmiToServer((uint16_t)S10RMI::Room_EnterPlayer_Req, roomenterplayerreq);
            });
        t.detach();

        break;
    }

    case S10RMI::EnterPlayer_Ack:
    {
        uint32_t hostID = 0;
        message.ByteArray::Read(hostID);
        uint64_t playerID = 0;
        message.ByteArray::Read(playerID);
        bool nicknameRes = false;
        std::string Nickname = message.ReadString(nicknameRes);
        GetClient().m_Logger.Information(fmt::format("Player joined => {0}, {1}", Nickname, playerID));

        auto it = RelayClient::GetInstance().Players.find(playerID);
        if (it != RelayClient::GetInstance().Players.end())
            it->second->HostId = hostID;
        else
            RelayClient::GetInstance().Players.insert(std::make_pair(playerID, new PlayerInfo(playerID, Nickname, hostID)));

        if (playerID != AuthClient::GetInstance().m_ClientlessPlayer.AccountId)
        {
            std::thread t([hostID]()
                {
                    std::this_thread::sleep_for(std::chrono::seconds(3));

                    NetMessage newmessage;

                    newmessage.ByteArray::Write((byte)1); //spawn req

                    newmessage.ByteArray::Write((uint64_t)0);
                    newmessage.ByteArray::Write((byte)0);

                    byte zero1[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));

                    newmessage.ByteArray::Write(8);
                    byte zero2[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));

                    newmessage.ByteArray::Write(2);
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));

                    newmessage.ByteArray::Write(3);
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));

                    newmessage.ByteArray::Write(3);
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));
                    newmessage.ByteArray::Write(zero2, sizeof(zero2));

                    newmessage.ByteArray::Write(0);
                    newmessage.ByteArray::Write((byte)1);
                    newmessage.Write("Clientless");
                    newmessage.ByteArray::Write((byte)0);
                    newmessage.Write("ETC");

                    byte zero3[] = { 0x00, 0x00 };
                    newmessage.ByteArray::Write(zero3, sizeof(zero3));
                    newmessage.ByteArray::Write(zero3, sizeof(zero3));
                    newmessage.ByteArray::Write(zero3, sizeof(zero3));

                    newmessage.ByteArray::Write(8);
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));
                    newmessage.ByteArray::Write(zero1, sizeof(zero1));

                    byte zero4[] = { 0x00, 0x00, 0x00, 0x00 };
                    newmessage.ByteArray::Write(zero4, sizeof(zero4));

                    NetMessage neweventpacket;

                    neweventpacket.WriteEnum(MessageType::RMI);
                    neweventpacket.WriteEnum(S10RMI::EVENT_PACKET);
                    neweventpacket.ByteArray::Write(false); //compressed

                    neweventpacket.Write((ByteArray)newmessage);

                    NetMessage newMessage2;
                    newMessage2.WriteEnum(MessageType::UnreliableRelay1);
                    newMessage2.WriteEnum(MessagePriority::Ring0);
                    newMessage2.WriteScalar(0);
                    newMessage2.WriteScalar(1);
                    newMessage2.ByteArray::Write(hostID);

                    newMessage2.Write((ByteArray)neweventpacket);

                    RelayClient::GetInstance().ProudToServer(newMessage2);
                });
            t.detach();
        }

        break;
    }

    case S10RMI::EVENT_PACKET:
    {
        bool compressed = false;
        message.ByteArray::Read(compressed);

        int64_t length = 0;

        message.ReadScalar(length);

        if (compressed)
        {
            size_t compressedLength = 0;
            byte* compressedData = message.ReadAll(compressedLength);
            NetMessage compressedmessage;
            compressedmessage.ByteArray::Write(compressedData, compressedLength);
            delete[] compressedData;
            message = NetZip::DecompressPacket(compressedmessage);
        }

        byte opcode = 0;
        message.ByteArray::Read(opcode);

        if (opcode == 1 || opcode == 2) //spawn
        {
            uint64_t ulongpeerid = 0;
            message.ByteArray::Read(ulongpeerid);
            LongPeerId longpeerid2(ulongpeerid);
            byte team = 0;
            message.ByteArray::Read(team);

            uint16_t compressedfloatx = 0;
            message.ByteArray::Read(compressedfloatx);
            uint16_t compressedfloaty = 0;
            message.ByteArray::Read(compressedfloaty);
            uint16_t compressedfloatz = 0;
            message.ByteArray::Read(compressedfloatz);
            uint16_t compressedfloatrota = 0;
            message.ByteArray::Read(compressedfloatrota);

            uint32_t costumecount = 0;
            message.ByteArray::Read(costumecount);
            for (uint32_t i = 0; i < costumecount; i++)
            {
                uint64_t uid = 0;
                message.ByteArray::Read(uid);
                int32_t color = 0;
                message.ByteArray::Read(color);
                int32_t unk = 0;
                message.ByteArray::Read(unk);
            }
            uint32_t skillcount = 0;
            message.ByteArray::Read(skillcount);
            for (uint32_t i = 0; i < skillcount; i++)
            {
                uint64_t uid = 0;
                message.ByteArray::Read(uid);
                int32_t color = 0;
                message.ByteArray::Read(color);
                int32_t unk = 0;
                message.ByteArray::Read(unk);
            }
            uint32_t weaponcount = 0;
            message.ByteArray::Read(weaponcount);
            for (uint32_t i = 0; i < weaponcount; i++)
            {
                uint64_t uid = 0;
                message.ByteArray::Read(uid);
                int32_t color = 0;
                message.ByteArray::Read(color);
                int32_t unk = 0;
                message.ByteArray::Read(unk);
            }
            uint32_t weaponcount2 = 0;
            message.ByteArray::Read(weaponcount2);
            for (uint32_t i = 0; i < weaponcount2; i++)
            {
                uint64_t uid = 0;
                message.ByteArray::Read(uid);
                int32_t color = 0;
                message.ByteArray::Read(color);
                int32_t unk = 0;
                message.ByteArray::Read(unk);
            }

            uint32_t currentweaponslot = 0;
            message.ByteArray::Read(currentweaponslot);
            byte gender = 0;
            message.ByteArray::Read(gender);
            bool usernameRes = false;
            std::string username = message.ReadString(usernameRes);
            byte unk2 = 0;
            message.ByteArray::Read(unk2);
            bool countryRes = false;
            std::string country = message.ReadString(countryRes);

            float currenthp = Extensions::ReadCompressedFloat(message);
            float maxhp = Extensions::ReadCompressedFloat(message);
            uint16_t compressedfloatunk3 = 0;
            message.ByteArray::Read(compressedfloatunk3);

            uint32_t valuecount = 0;
            message.ByteArray::Read(valuecount);
            for (uint32_t i = 0; i < valuecount; i++)
            {
                float value1 = 0.0f;
                message.ByteArray::Read(value1);
                float value2 = 0.0f;
                message.ByteArray::Read(value2);
            }

            byte unk4 = 0;
            message.ByteArray::Read(unk4);
            byte unk5 = 0;
            message.ByteArray::Read(unk5);
            byte unk6 = 0;
            message.ByteArray::Read(unk6);
            byte unk7 = 0;
            message.ByteArray::Read(unk7);

            auto it = RelayClient::GetInstance().Players.find(longpeerid2.AccountId);
            if (it != RelayClient::GetInstance().Players.end())
            {
                PlayerInfo* plr = it->second;
                plr->m_LongPeerId = longpeerid2;
                if (plr->Nickname.empty())
                    plr->Nickname = username;

                GetClient().m_Logger.Information(fmt::format("Spawn P2P => accid = {0}, peerid = {1}, team = {2}, gender = {3}, username = {4}, country = {5}, maxhp = {6}",
                    longpeerid2.AccountId, (uint16_t)longpeerid2.m_PeerId, team, gender, username, country, maxhp));
            }
        }
        else
        {
            size_t dataSize = 0;
            byte* data = message.ReadAll(dataSize);
            //Client.Logger.Verbose($"P2P<{opcode}>({message.RelayFrom}) | {data.ToHexString()}");
            delete[] data;
        }

        break;
    }

    case S10RMI::EVENT_P2P_EVENTMESSAGE:
    {
        uint32_t eventtype = 0;
        message.ByteArray::Read(eventtype);

        if (eventtype == 13) //chat
        {
            uint64_t accid = 0;
            message.ByteArray::Read(accid);

            auto it = RelayClient::GetInstance().Players.find(accid);
            if (it != RelayClient::GetInstance().Players.end())
            {
                PlayerInfo* plr = it->second;
                int32_t gametime = 0;
                message.ByteArray::Read(gametime);
                uint16_t value = 0;
                message.ByteArray::Read(value);

                bool chatmsgRes = false;
                std::string chatmsg = message.ReadString(chatmsgRes);

                GetClient().m_Logger.Information(fmt::format("P2P Message from {0}<{1}> => {2}", plr->Nickname, plr->AccountId, chatmsg));
            }
        }

        break;
    }

    default:
    {
        if (rmiID > 64000 && rmiID < 65000)
            GetClient().m_Logger.Warning(fmt::format("Recieved unknown Proud Op Code {0}", (ProudOpCode)rmiID));

        break;
    }
    }
}
