#include "GameClient.h" //cant be included below because of shitty windows header
#include "AuthClient.h" //cant be included below because of shitty windows header
#include "ChatClient.h" //cant be included below because of shitty windows header
#include "RelayClient.h" //cant be included below because of shitty windows header
#include <string>
#include <fmt/format.h>
#include <array>
#include <thread>
#include <chrono>

#include "GameHandler.h"
#include "../BaseLib/StringUtils.h"
#include "S4RMI.h"
#include "../BaseLib/Version.h"
#include "Config.h"

using namespace Nettention::Proud;
using namespace BaseLib;
using namespace BaseLib::Utils;

extern uint32_t ChannelId;
extern byte RoomId;

GameHandler::GameHandler(NetClient* client)
    : Handler(client)
{
}

void GameHandler::InitHandler()
{
    NetMessage loginreq;
    loginreq.ByteArray::Write(0);
    loginreq.Write(StringUtils::EmptyString);
    Version ver;
    loginreq.Write(ver);
    loginreq.ByteArray::Write((int16_t)0);
    loginreq.ByteArray::Write((byte)0);
    loginreq.ByteArray::Write((byte)0);
    loginreq.ByteArray::Write(AuthClient::GetInstance().m_ClientlessPlayer.AccountId);
    loginreq.Write(StringUtils::EmptyString);
    loginreq.Write(StringUtils::EmptyString);
    loginreq.ByteArray::Write(false);
    loginreq.Write(StringUtils::EmptyString);
    loginreq.ByteArray::Write(0);
    loginreq.Write(StringUtils::EmptyString);
    loginreq.Write(AuthClient::GetInstance().m_ClientlessPlayer.AuthToken);
    loginreq.Write(AuthClient::GetInstance().m_ClientlessPlayer.NewToken);
    loginreq.Write(AuthClient::GetInstance().m_ClientlessPlayer.DateTime);

    GetClient().RmiToServer((uint16_t)S10RMI::Login_Request_Req, loginreq);
}

void GameHandler::HandleRMI(NetMessage& message, uint16_t rmiID)
{
    switch ((S10RMI)rmiID)
    {
    case S10RMI::Login_Reguest_Ack:
    {
        NetMessage rescheck;
        rescheck.Write(StringUtils::EmptyString);

        GetClient().RmiToServer((uint16_t)S10RMI::Check_hash_Key_value_req, rescheck);

        uint64_t accid = 0;
        message.ByteArray::Write(accid);
        uint32_t result = 0;
        message.ByteArray::Write(result);

        if (result == 6)
        {
            GetClient().m_Logger.Warning("GameServer Login | No characters found. Creating one");

            NetMessage characterfirstcreatereq;
            characterfirstcreatereq.Write("Clientless123123");
            characterfirstcreatereq.ByteArray::Write((byte)1);

            std::array<uint32_t, 8> defaultItems
            {
                1000001,//1000001
                0,
                1020027,//1020027
                1030022,//1030022
                1040027,//1040027
                1050025,//1050025
                0,
                0
            };

            for (uint32_t i : defaultItems)
                characterfirstcreatereq.ByteArray::Write(i);

            GetClient().RmiToServer((uint16_t)S10RMI::Character_FirstCreate_Req, characterfirstcreatereq);
        }
        if (!ChatClient::GetInstance().IsConnected())
        {
            std::thread t = std::thread([]()
                {
                    Config::SetChatConfig(GameClient::GetInstance().ChatEndPoint, true);
                    ChatClient::GetInstance().Connect(Config::ChatEndPoint, Config::ChatNonBlocking);
                });
            t.detach();
        }

        break;
    }

    case S10RMI::Channel_ListInfo_Ack:
    {
        for (auto& x : GameClient::GetInstance().Channels)
        {
            delete x.second;
        }
        GameClient::GetInstance().Channels.clear();

        uint32_t count = 0;
        message.ByteArray::Read(count);

        GetClient().m_Logger.Information("Got channel list: ");

        for (uint32_t i = 0; i < count; i++)
        {
            ChannelInfo* channel = new ChannelInfo();
            message.ByteArray::Read(channel->ID);
            message.ByteArray::Read(channel->PlayerCount);
            message.ByteArray::Read(channel->PlayerLimit);
            message.ByteArray::Read(channel->IsClub);
            bool nameRes = false;
            channel->Name = message.ReadString(nameRes);
            bool rankRes = false;
            channel->Rank = message.ReadString(rankRes);
            bool descriptionRes = false;
            channel->Description = message.ReadString(descriptionRes);
            message.ByteArray::Read(channel->color);
            message.ByteArray::Read(channel->min_level);
            message.ByteArray::Read(channel->max_level);
            message.ByteArray::Read(channel->min_kdrate);
            message.ByteArray::Read(channel->max_kdrate);

            if (channel->ID != 0 && channel->IsClub == 0)
            {
                GameClient::GetInstance().Channels.insert(std::make_pair(channel->ID, channel));

                GetClient().m_Logger.Information(fmt::format("{0} => {1}", channel->ID, channel->Name));
            }
        }

        if (GameClient::GetInstance().Channels.size() == 1)
        {
            ChannelInfo* channel = GameClient::GetInstance().Channels.begin()->second;

            ChannelId = channel->ID;

            NetMessage channelenterreq;
            channelenterreq.ByteArray::Write((uint32_t)channel->ID);

            GetClient().RmiToServer((uint16_t)S10RMI::Channel_Enter_Req, channelenterreq);

            std::thread t = std::thread([]()
                {
                    NetMessage channelinforeq;
                    channelinforeq.ByteArray::Write((byte)4);

                    std::this_thread::sleep_for(std::chrono::seconds(1));
                        GameClient::GetInstance().RmiToServer((uint16_t)S10RMI::Channel_Info_Req, channelinforeq);
                });
            t.detach();
        }

        break;
    }

    case S10RMI::Room_ListInfo_Ack_2:
    {
        for (auto& x : GameClient::GetInstance().Rooms)
        {
            delete x.second;
        }
        GameClient::GetInstance().Rooms.clear();

        uint32_t count = 0;
        message.ByteArray::Read(count);

        GetClient().m_Logger.Information("Got room list: ");

        for (uint32_t i = 0; i < count; i++)
        {
            RoomInfo* room = new RoomInfo();
            message.ByteArray::Read(room->RoomId);
            message.ByteArray::Read(room->State);
            message.ByteArray::Read(room->GameRule);
            message.ByteArray::Read(room->Map);
            message.ByteArray::Read(room->PlayerCount);
            message.ByteArray::Read(room->PlayerLimit);
            message.ByteArray::Read(room->WeaponLimit);

            bool passwordRes = false;
            room->Password = message.ReadString(passwordRes);

            bool nameRes = false;
            room->Name = message.ReadString(nameRes);
            message.ByteArray::Read(room->SpectatorEnabled);
            message.ByteArray::Read(room->Unk1);
            message.ByteArray::Read(room->IsRandom);
            message.ByteArray::Read(room->FMBurnMode);
            message.ByteArray::Read(room->Unk4);
            message.ByteArray::Read(room->Unk5);

            GameClient::GetInstance().Rooms.insert(std::make_pair(room->RoomId, room));

            GetClient().m_Logger.Information(fmt::format("{0} => {1}", room->RoomId, room->Name));
        }

        if (GameClient::GetInstance().Rooms.size() == 1)
        {
            RoomInfo* room = GameClient::GetInstance().Rooms.begin()->second;

            RoomId = room->RoomId;

            NetMessage roomenterreq;
            roomenterreq.ByteArray::Write((uint32_t)room->RoomId);
            roomenterreq.Write(room->Password);
            roomenterreq.ByteArray::Write((byte)0);
            roomenterreq.ByteArray::Write((byte)0);

            GameClient::GetInstance().RmiToServer((uint16_t)S10RMI::Room_Enter_Req, roomenterreq);
        }

        break;
    }

    case S10RMI::Room_EnterRoomInfo_Ack_2:
    {
        uint32_t roomid = 0;
        message.ByteArray::Read(roomid);
        uint32_t gamerule = 0;
        message.ByteArray::Read(gamerule);
        byte mapid = 0;
        message.ByteArray::Read(mapid);
        byte playerlimit = 0;
        message.ByteArray::Read(playerlimit);
        uint32_t gamestate = 0;
        message.ByteArray::Read(gamestate);
        uint32_t gametimestate = 0;
        message.ByteArray::Read(gametimestate);
        uint32_t timelimit = 0;
        message.ByteArray::Read(timelimit);
        uint32_t arcadedifficulty = 0;
        message.ByteArray::Read(arcadedifficulty);
        uint32_t timesync = 0;
        message.ByteArray::Read(timesync);
        uint32_t scorelimit = 0;
        message.ByteArray::Read(scorelimit);
        bool arcademovie = 0;
        message.ByteArray::Read(arcademovie);
        IPEndPoint relayendpoint;
        message.ReadStringEndPoint(relayendpoint);

        std::thread t = std::thread([relayendpoint]()
            {
                Config::SetRelayConfig(relayendpoint, true);
                for (auto& x : RelayClient::GetInstance().Players)
                {
                    delete x.second;
                }
                RelayClient::GetInstance().Players.clear();
                RelayClient::GetInstance().Players
                    .insert(std::make_pair(
                        AuthClient::GetInstance().m_ClientlessPlayer.AccountId,
                        new PlayerInfo(AuthClient::GetInstance().m_ClientlessPlayer.AccountId, "Clientless", 0u)));
                RelayClient::GetInstance().Connect(Config::RelayEndPoint, Config::RelayNonBlocking);
            });
        t.detach();

        break;
    }

    case S10RMI::Room_PlayerInfoListForEnterPlayer_Ack:
    {
        uint32_t playerCount = 0;
        message.ByteArray::Read(playerCount);

        for (uint32_t index = 0; index < playerCount; index++)
        {
            uint32_t unk1 = 0;
            message.ByteArray::Read(unk1);
            uint64_t playerId2 = 0;
            message.ByteArray::Read(playerId2);
            byte unk2 = 0;
            message.ByteArray::Read(unk2);
            bool nicknameRes = false;
            std::string nickname = message.ReadString(nicknameRes);
            byte unk3 = 0;
            message.ByteArray::Read(unk3);
            bool unk4 = false;
            message.ByteArray::Read(unk4);
            byte unk5 = 0;
            message.ByteArray::Read(unk5);

            std::thread t = std::thread([playerId2, nickname]()
                {
                    std::this_thread::sleep_for(std::chrono::seconds(2));

                    auto it = RelayClient::GetInstance().Players.find(playerId2);
                    if (it != RelayClient::GetInstance().Players.end())
                    {
                        PlayerInfo* plr = it->second;
                        if (plr->Nickname.empty())
                            plr->Nickname = nickname;
                    }
                    else
                    {
                        RelayClient::GetInstance().Players.insert(std::make_pair(playerId2, new PlayerInfo(playerId2, nickname)));
                    }
                });
            t.detach();
        }

        break;
    }

    case S10RMI::Room_LeavePlayerInfo_Ack:
    {
        uint64_t playerId = 0;
        message.ByteArray::Read(playerId);

        auto it = RelayClient::GetInstance().Players.find(playerId);
        if (it != RelayClient::GetInstance().Players.end())
        {
            PlayerInfo* plr = it->second;
            delete plr;

            RelayClient::GetInstance().Players.unsafe_erase(playerId);
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
