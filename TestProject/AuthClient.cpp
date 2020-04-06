#include <exception>

#include "AuthClient.h"
#include "Config.h"
#include "../BaseLib/IPEndPoint.h"

using namespace Nettention::Proud;
using namespace BaseLib;

AuthClient* AuthClient::Instance = nullptr;

AuthClient::AuthClient()
	: NetClient(ServerType::Auth), p_Handler((NetClient*)this)
{
	OnConnectionComplete = std::bind(&AuthHandler::InitHandler, &p_Handler);
	OnRMIRecieve = std::bind(&AuthHandler::HandleRMI, &p_Handler, std::placeholders::_1, std::placeholders::_2);
}

void AuthClient::Init()
{
	static AuthClient instance;

	Instance = &instance;
}

AuthClient& AuthClient::GetInstance()
{
	if (Instance == nullptr)
		throw std::exception("Instance is null!");

	return *Instance;
}
