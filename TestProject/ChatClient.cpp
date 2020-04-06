#include <exception>

#include "ChatClient.h"
#include "Config.h"
#include "../BaseLib/IPEndPoint.h"

using namespace Nettention::Proud;
using namespace BaseLib;

ChatClient* ChatClient::Instance = nullptr;

ChatClient::ChatClient()
	: NetClient(ServerType::Chat), p_Handler((NetClient*)this)
{
	OnConnectionComplete = std::bind(&ChatHandler::InitHandler, &p_Handler);
	OnRMIRecieve = std::bind(&ChatHandler::HandleRMI, &p_Handler, std::placeholders::_1, std::placeholders::_2);
}

void ChatClient::Init()
{
	static ChatClient instance;

	Instance = &instance;
}

ChatClient& ChatClient::GetInstance()
{
	if (Instance == nullptr)
		throw std::exception("Instance is null!");

	return *Instance;
}
