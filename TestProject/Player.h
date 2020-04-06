#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

class Player
{
public:
	uint64_t AccountId = 0;
};

class ClientlessPlayer : public Player
{
public:
	std::string AuthToken;
	std::string NewToken;
	std::string DateTime;
};
