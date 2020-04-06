#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include "AuthClient.h" //cant be included below because of shitty windows header
#include "GameClient.h" //cant be included below because of shitty windows header
#include "ChatClient.h" //cant be included below because of shitty windows header
#include "RelayClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <functional>
#include <fmt/format.h>

#include "../ProudNetClient/NetMessage.h"
#include "S4RMI.h"
#include "../BaseLib/IPEndPoint.h"
#include "Config.h"

using namespace Nettention::Proud;
using namespace BaseLib;

uint32_t ServerId = 0;
uint32_t ChannelId = 0;
byte RoomId = 0;

int main()
{
	Config::SetAuthConfig(IPEndPoint("127.0.0.1", 28002), true);
	AuthClient::Init();
	AuthClient::GetInstance().Connect(Config::AuthEndPoint, Config::AuthNonBlocking);
	GameClient::Init();
	ChatClient::Init();
	RelayClient::Init();

	std::cin.get();

	return 0;
}