#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

#include "../BaseLib/IPEndPoint.h"

class Config
{
public:
	static void SetAuthConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking = false);
	static void SetGameConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking = false);
	static void SetChatConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking = false);
	static void SetRelayConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking = false);

	static BaseLib::IPEndPoint AuthEndPoint;
	static bool AuthNonBlocking;

	static BaseLib::IPEndPoint GameEndPoint;
	static bool GameNonBlocking;

	static BaseLib::IPEndPoint ChatEndPoint;
	static bool ChatNonBlocking;

	static BaseLib::IPEndPoint RelayEndPoint;
	static bool RelayNonBlocking;
};