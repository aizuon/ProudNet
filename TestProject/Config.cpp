#include "Config.h"

using namespace BaseLib;

IPEndPoint Config::AuthEndPoint;
bool Config::AuthNonBlocking = false;

IPEndPoint Config::GameEndPoint;
bool Config::GameNonBlocking = false;

IPEndPoint Config::ChatEndPoint;
bool Config::ChatNonBlocking = false;

IPEndPoint Config::RelayEndPoint;
bool Config::RelayNonBlocking = false;

void Config::SetAuthConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking)
{
	AuthEndPoint = endPoint;
	AuthNonBlocking = nonBlocking;
}

void Config::SetGameConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking)
{
	GameEndPoint = endPoint;
	GameNonBlocking = nonBlocking;
}

void Config::SetChatConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking)
{
	ChatEndPoint = endPoint;
	ChatNonBlocking = nonBlocking;
}

void Config::SetRelayConfig(const BaseLib::IPEndPoint& endPoint, bool nonBlocking)
{
	RelayEndPoint = endPoint;
	RelayNonBlocking = nonBlocking;
}
