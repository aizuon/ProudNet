#pragma once
#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>
#include <concurrent_unordered_map.h>

#include "../BaseLib/IPEndPoint.h"
#include "GameHandler.h"
#include "ChannelInfo.h"
#include "RoomInfo.h"

class GameClient : public Nettention::Proud::NetClient
{
public:
	static void Init();
	static GameClient& GetInstance();

	BaseLib::IPEndPoint ChatEndPoint;

	concurrency::concurrent_unordered_map<uint32_t, ChannelInfo*> Channels;
	concurrency::concurrent_unordered_map<byte, RoomInfo*> Rooms;

private:
	GameClient();

	GameHandler p_Handler;
	static GameClient* Instance;
};