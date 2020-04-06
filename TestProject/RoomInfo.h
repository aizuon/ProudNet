#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include <chrono>
#include <boost/date_time/gregorian/gregorian.hpp>

struct RoomInfo
{
    int16_t FMBurnMode = 0;
    int32_t GameRule = 0;
    uint32_t GameState = 0;
    uint32_t GameTimeState = 0;
    int32_t IsRandom = 0;
    boost::gregorian::date JoinTime;
    byte Map = 0;
    std::string Name;
    std::string Password;

    //new Room?
    bool PasswordYesNo = false;
    byte PlayerCount = 0;
    byte PlayerLimit = 0;
    byte RoomId = 0;
    bool SpectatorEnabled = false;
    byte State = 0;
    std::chrono::duration<uint64_t> TimeLimit;
    std::chrono::duration<uint64_t> TimeSync;
    int16_t unk1 = 0;
    byte Unk1 = 0;
    byte unk2 = 0;
    int64_t Unk4 = 0;
    int16_t Unk5 = 0;
    int32_t WeaponLimit = 0;
};