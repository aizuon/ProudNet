#include <exception>

#include "RelayClient.h"
#include "Config.h"
#include "../BaseLib/IPEndPoint.h"

using namespace Nettention::Proud;
using namespace BaseLib;

RelayClient* RelayClient::Instance = nullptr;

RelayClient::RelayClient()
	: NetClient(ServerType::Relay), p_Handler((NetClient*)this)
{
	OnConnectionComplete = std::bind(&RelayHandler::InitHandler, &p_Handler);
	OnRMIRecieve = std::bind(&RelayHandler::HandleRMI, &p_Handler, std::placeholders::_1, std::placeholders::_2);
}

void RelayClient::Init()
{
	static RelayClient instance;

	Instance = &instance;
}

RelayClient& RelayClient::GetInstance()
{
	if (Instance == nullptr)
		throw std::exception("Instance is null!");

	return *Instance;
}
