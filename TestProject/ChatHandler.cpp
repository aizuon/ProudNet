#include "ChatClient.h" //cant be included below because of shitty windows header
#include "AuthClient.h" //cant be included below because of shitty windows header
#include "GameClient.h" //cant be included below because of shitty windows header
#include <string>
#include <fmt/format.h>
#include <vector>

#include "ChatHandler.h"
#include "../BaseLib/StringUtils.h"
#include "S4RMI.h"

using namespace Nettention::Proud;
using namespace BaseLib::Utils;

ChatHandler::ChatHandler(NetClient* client)
    : Handler(client)
{
}

void ChatHandler::InitHandler()
{
    NetMessage loginreq;
    loginreq.ByteArray::Write(AuthClient::GetInstance().m_ClientlessPlayer.AccountId);
    loginreq.Write(StringUtils::EmptyString);
    loginreq.Write(StringUtils::EmptyString);

    GetClient().RmiToServer((uint16_t)S10RMI::Chat_Login_Req, loginreq);
}

void ChatHandler::HandleRMI(NetMessage& message, uint16_t rmiID)
{
    switch ((S10RMI)rmiID)
    {
    case S10RMI::Chat_Login_Ack:
    {
        NetMessage channelinforeq;
        channelinforeq.ByteArray::Write((byte)5);

        GameClient::GetInstance().RmiToServer((uint16_t)S10RMI::Channel_Info_Req, channelinforeq);

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
