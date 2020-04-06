#pragma once
#include <Windows.h>
#include <string>

namespace BaseLib
{
	namespace Utils
	{
		class GuidUtils
		{
		public:
			static std::string ToString(const GUID& guid);
			static GUID FromString(const std::string& str);
		};
	}
}