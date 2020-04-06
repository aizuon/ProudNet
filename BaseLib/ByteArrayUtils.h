#pragma once
#include <Windows.h>
#include <string>

namespace BaseLib
{
	namespace Utils
	{
		class ByteArrayUtils
		{
		public:
			static std::string ToHexString(byte* byteArray, size_t length);
		};
	}
}