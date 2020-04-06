#pragma once
#include <string>

namespace BaseLib
{
	namespace Utils
	{
		class StringUtils
		{
		public:
			static std::wstring ANSI2UNICODE(const std::string& str);
			static std::string UNICODE2ANSI(const std::wstring& wstr);

			static std::string EmptyString;
		};
	}
}