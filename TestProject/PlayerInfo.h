#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

#include "LongPeerId.h"

struct PlayerInfo
{
    PlayerInfo(uint64_t accid, const std::string& nick, uint32_t hostid = 0)
        : m_LongPeerId(0UL)
    {
        AccountId = accid;
        Nickname = nick;
        HostId = hostid;
    }

	uint32_t HostId;
	uint64_t AccountId;
	LongPeerId m_LongPeerId;
	std::string Nickname;
};