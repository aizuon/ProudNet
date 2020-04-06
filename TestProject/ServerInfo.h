#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

#include "../BaseLib/IPEndPoint.h"

struct ChatServerInfo
{
	BaseLib::IPEndPoint EndPoint;
	uint16_t GroupId = 0;
};

struct ServerInfo
{
	BaseLib::IPEndPoint EndPoint;
	BaseLib::IPEndPoint ChatEndPoint;
	uint16_t GroupId = 0;
	uint32_t Id = 0;
	bool IsEnabled = false;
	std::string Name;
	uint16_t PlayerLimit = 0;
	uint16_t PlayerOnline = 0;
	uint32_t Type = 0;
};