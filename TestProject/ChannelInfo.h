#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>

struct ChannelInfo
{
    uint32_t color = 0;
    std::string Description;
    uint16_t ID = 0;
    int32_t IsClub = 0;
    int32_t max_kdrate = 0;
    int32_t max_level = 0;
    int32_t min_kdrate = 0;
    int32_t min_level = 0;
    std::string Name;
    uint16_t PlayerCount = 0;
    uint16_t PlayerLimit = 0;
    std::string Rank;
};