#include <Windows.h>

#include "StringUtils.h"

namespace BaseLib
{
	namespace Utils
	{
		std::string StringUtils::EmptyString;

		std::wstring StringUtils::ANSI2UNICODE(const std::string& str)
		{
			int size_needed = MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), NULL, 0);
			std::wstring wstrTo(size_needed, 0);
			MultiByteToWideChar(CP_ACP, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
			return wstrTo;
		}

		std::string StringUtils::UNICODE2ANSI(const std::wstring& wstr)
		{
			int size_needed = WideCharToMultiByte(CP_ACP, 0, &wstr[0], -1, NULL, 0, NULL, NULL);
			std::string strTo(size_needed, 0);
			WideCharToMultiByte(CP_ACP, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
			return strTo;
		}
	}
}