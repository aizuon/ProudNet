#include <exception>

#include "GameClient.h"
#include "Config.h"
#include "../BaseLib/IPEndPoint.h"

using namespace Nettention::Proud;
using namespace BaseLib;

GameClient* GameClient::Instance = nullptr;

GameClient::GameClient()
	: NetClient(ServerType::Game), p_Handler((NetClient*)this)
{
	OnConnectionComplete = std::bind(&GameHandler::InitHandler, &p_Handler);
	OnRMIRecieve = std::bind(&GameHandler::HandleRMI, &p_Handler, std::placeholders::_1, std::placeholders::_2);
}

void GameClient::Init()
{
	static GameClient instance;

	Instance = &instance;
}

GameClient& GameClient::GetInstance()
{
	if (Instance == nullptr)
		throw std::exception("Instance is null!");

	return *Instance;
}
