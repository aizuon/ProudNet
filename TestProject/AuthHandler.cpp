#include "AuthClient.h" //cant be included below because of shitty windows header
#include "GameClient.h" //cant be included below because of shitty windows header
#include <string>
#include <fmt/format.h>
#include <vector>
#include <thread>

#include "AuthHandler.h"
#include "../BaseLib/StringUtils.h"
#include "S4RMI.h"
#include "Config.h"

using namespace Nettention::Proud;
using namespace BaseLib;
using namespace BaseLib::Utils;

extern uint32_t ServerId;

AuthHandler::AuthHandler(NetClient* client)
	: Handler(client)
{
}

void AuthHandler::InitHandler()
{
    NetMessage logineureq;
    logineureq.Write("clientless123123");
    logineureq.Write("clientless123123");
    logineureq.Write(StringUtils::EmptyString);
    logineureq.Write(StringUtils::EmptyString);
    logineureq.ByteArray::Write(0);
    logineureq.ByteArray::Write(0);
    logineureq.ByteArray::Write(0);
    logineureq.Write(StringUtils::EmptyString);
    logineureq.ByteArray::Write(0);
    logineureq.Write(StringUtils::EmptyString);
    logineureq.Write(StringUtils::EmptyString);
    logineureq.Write(StringUtils::EmptyString); //token
    logineureq.Write(StringUtils::EmptyString);
    logineureq.Write(StringUtils::EmptyString);
    logineureq.Write(StringUtils::EmptyString);

    GetClient().RmiToServer((uint16_t)S10RMI::Login_EU_Req, logineureq);
}

void AuthHandler::HandleRMI(NetMessage& message, uint16_t rmiID)
{
    switch ((S10RMI)rmiID)
    {
    case S10RMI::Login_EU_Ack:
    {
        uint64_t accid = 0;
        message.ByteArray::Read(accid);
        AuthClient::GetInstance().m_ClientlessPlayer.AccountId = accid;

        uint32_t clientid = 0;
        message.ByteArray::Read(clientid);
        bool unk1Result = false;
        std::string unk1 = message.ReadString(unk1Result);
        bool clientid2Result = false;
        std::string clientid2 = message.ReadString(clientid2Result);
        byte result = 0;
        message.ByteArray::Read(result);
        if (result != 0)
            GetClient().m_Logger.Error("Login_EU_Ack result is not OK");
        bool unk2Result = false;
        std::string unk2 = message.ReadString(unk2Result);
        bool banneduntilResult = false;
        std::string banneduntil = message.ReadString(banneduntilResult);
        bool munknow04Result = false;
        std::string munknow04 = message.ReadString(munknow04Result);
        bool AuthtokenResult = false;
        std::string Authtoken = message.ReadString(AuthtokenResult);
        AuthClient::GetInstance().m_ClientlessPlayer.AuthToken = Authtoken;
        bool newtokenResult = false;
        std::string newtoken = message.ReadString(newtokenResult);
        AuthClient::GetInstance().m_ClientlessPlayer.NewToken = newtoken;
        bool datetimeResult = false;
        std::string datetime = message.ReadString(datetimeResult);
        AuthClient::GetInstance().m_ClientlessPlayer.DateTime = datetime;

        NetMessage serverlistreq;
        GetClient().RmiToServer((uint16_t)S10RMI::ServerList_Req, serverlistreq);

        break;
    }

    case S10RMI::ServerList_Ack:
    {
        for (auto& x : AuthClient::GetInstance().Servers)
        {
            delete x.second;
        }
        AuthClient::GetInstance().Servers.clear();

        uint32_t count = 0;
        message.ByteArray::Read(count);

        GetClient().m_Logger.Information("Got server list: ");

        std::vector<ServerInfo*> gameservers;
        gameservers.reserve(count / 2);
        std::vector<ChatServerInfo*> chatservers;
        chatservers.reserve(count / 2);

        for (uint32_t i = 0; i < count; i++)
        {
            ServerInfo* server = new ServerInfo();
            message.ByteArray::Read(server->IsEnabled);
            message.ByteArray::Read(server->Id);
            message.ByteArray::Read(server->Type);
            bool nameRes = false;
            server->Name = message.ReadString(nameRes);
            message.ByteArray::Read(server->PlayerLimit);
            message.ByteArray::Read(server->PlayerOnline);
            message.ReadEndPoint(server->EndPoint);
            message.ByteArray::Read(server->GroupId);

            if (server->Type == 3)
            {
                gameservers.push_back(server);

                GetClient().m_Logger.Information(fmt::format("{0} => {1} {2}:{3}", server->Id, server->Name, server->EndPoint.IP, server->EndPoint.Port));
            }
            else if (server->Type == 5)
            {
                ChatServerInfo* chatserver = new ChatServerInfo();
                chatserver->EndPoint = server->EndPoint;
                chatserver->GroupId = server->GroupId;
                delete server;
                chatservers.push_back(chatserver);
            }
        }

        for (auto game : gameservers)
        {
            for (auto chat : chatservers)
            {
                if (game->GroupId == chat->GroupId)
                {
                    game->ChatEndPoint = chat->EndPoint;

                    break;
                }
            }

            AuthClient::GetInstance().Servers.insert(std::make_pair(game->Id, game));
        }

        for (auto chat : chatservers)
        {
            delete chat;
        }

        if (AuthClient::GetInstance().Servers.size() == 1)
        {
            ServerInfo* server = AuthClient::GetInstance().Servers.begin()->second;

            ServerId = server->Id;

            std::thread t = std::thread([server]()
                {
                    Config::SetGameConfig(server->EndPoint, true);
                    GameClient::GetInstance().ChatEndPoint = server->ChatEndPoint;
                    GameClient::GetInstance().Connect(Config::GameEndPoint, Config::GameNonBlocking);
                });
            t.detach();
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
