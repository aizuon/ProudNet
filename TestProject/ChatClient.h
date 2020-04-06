#pragma once
#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>

#include "../BaseLib/IPEndPoint.h"
#include "ChatHandler.h"
#include "Player.h"

class ChatClient : public Nettention::Proud::NetClient
{
public:
	static void Init();
	static ChatClient& GetInstance();

private:
	ChatClient();

	ChatHandler p_Handler;
	static ChatClient* Instance;
};