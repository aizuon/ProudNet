#pragma once
#include "../ProudNetClient/NetClient.h" //cant be included below because of shitty windows header
#include <Windows.h>
#include <cstdint>
#include <concurrent_unordered_map.h>

#include "../BaseLib/IPEndPoint.h"
#include "AuthHandler.h"
#include "ServerInfo.h"
#include "Player.h"

class AuthClient : public Nettention::Proud::NetClient
{
public:
	static void Init();
	static AuthClient& GetInstance();

	ClientlessPlayer m_ClientlessPlayer;
	concurrency::concurrent_unordered_map<uint32_t, ServerInfo*> Servers;

private:
	AuthClient();

	AuthHandler p_Handler;
	static AuthClient* Instance;
};