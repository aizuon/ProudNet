#pragma once
#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>
#include <concurrent_unordered_map.h>

#include "../BaseLib/IPEndPoint.h"
#include "RelayHandler.h"
#include "PlayerInfo.h"

class RelayClient : public Nettention::Proud::NetClient
{
public:
	static void Init();
	static RelayClient& GetInstance();

	concurrency::concurrent_unordered_map<uint64_t, PlayerInfo*> Players;

private:
	RelayClient();

	RelayHandler p_Handler;
	static RelayClient* Instance;
};