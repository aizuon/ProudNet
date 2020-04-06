#include "Extensions.h"

using namespace Nettention::Proud;
using namespace BaseLib;

int16_t Extensions::Compress(float value)
{
	int32_t tmp = *(int32_t*)&value;
	int64_t value1 = (tmp & 0x80000000) >> 0x1F;
	int32_t value2 = ((tmp & 0x7F800000) >> 0x17) - 0x7F;
	int32_t value3 = tmp & 0x7FFFFF;

	return (int16_t)((value1 << 0xF) | ((value2 + 0x7) << 0x9) | (value3 >> 0xE));
}

float Extensions::Decompress(uint16_t value)
{
	int32_t result = ((value & 0x1FF) << 14) | ((((value & 0x7F00) >> 9) - 7 + 127) << 23) |
		(((value & 0x8000) >> 15) << 31);

	return *(float*)&result;
}

float Extensions::Decompress(int16_t value)
{
	int32_t result = ((value & 0x1FF) << 14) | ((((value & 0x7F00) >> 9) - 7 + 127) << 23) |
		(((value & 0x8000) >> 15) << 31);

	return *(float*)&result;
}

float Extensions::ReadCompressedFloat(NetMessage& m)
{
	int16_t compressed = 0;
	m.ByteArray::Read(compressed);
	return Decompress(compressed);
}

Vector3 Extensions::ReadCompressedVector3(NetMessage& m)
{
	return Vector3(ReadCompressedFloat(m), ReadCompressedFloat(m), ReadCompressedFloat(m));
}

void Extensions::WriteCompressed(NetMessage& m, float value)
{
	m.ByteArray::Write(Compress(value));
}
