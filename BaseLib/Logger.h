#pragma once
#include <Windows.h>
#include <string>
#include <mutex>

namespace BaseLib
{
	class Logger
	{
	public:
		Logger(const std::string& name);

		void Verbose(const std::string& log) const;
		void Debug(const std::string& log) const;
		void Warning(const std::string& log) const;
		void Information(const std::string& log) const;
		void Error(const std::string& log) const;

	private:
		std::string Name;
		static std::mutex mtx;
		static HANDLE Console;
		const static std::string EndLine;
		const static std::string VerboseString;
		const static std::string DebugString;
		const static std::string WarningString;
		const static std::string InformationString;
		const static std::string ErrorString;

		enum class ConsoleColor : byte
		{
			Black,
			Dark_Blue,
			Green,
			Blue_Grey,
			Brown,
			Purple,
			Khaki,
			Light_Grey,
			Grey,
			Light_Blue,
			Light_Green,
			Cyan,
			Red,
			Pink,
			Yellow,
			White
		};
	};
}