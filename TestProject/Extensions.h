#pragma once
#include <Windows.h>
#include <cstdint>

#include "../ProudNetClient/NetMessage.h"
#include "../BaseLib/Vector3.h"

class Extensions
{
public:
	static int16_t Compress(float value);
	static float Decompress(uint16_t value);
	static float Decompress(int16_t value);
	static float ReadCompressedFloat(Nettention::Proud::NetMessage& m);
	static BaseLib::Vector3 ReadCompressedVector3(Nettention::Proud::NetMessage& m);
	static void WriteCompressed(Nettention::Proud::NetMessage& m, float value);
};