#include <iostream>
#include <fmt/format.h>
#include <boost/algorithm/string.hpp>

#include "Logger.h"

namespace BaseLib
{
	HANDLE Logger::Console = NULL;
	std::mutex Logger::mtx;
	const std::string Logger::EndLine = "\n";
	const std::string Logger::VerboseString = "[ VERBOSE ] ";
	const std::string Logger::DebugString = "[ DEBUG ] ";
	const std::string Logger::WarningString = "[ WARNING ] ";
	const std::string Logger::InformationString = "[ INFORMATION ] ";
	const std::string Logger::ErrorString = "[ ERROR ] ";

	Logger::Logger(const std::string& name)
	{
		Name = fmt::format("[ {0} ] ", boost::to_upper_copy(name));

		if (Console == NULL)
			Console = GetStdHandle(STD_OUTPUT_HANDLE);
	}

	void Logger::Verbose(const std::string& log) const
	{
#if _DEBUG
		std::lock_guard<std::mutex> lock(mtx);
		SetConsoleTextAttribute(Console, static_cast<WORD>(ConsoleColor::White));
		std::cout << Name << VerboseString << log << EndLine;
#endif
	}

	void Logger::Debug(const std::string& log) const
	{
#if _DEBUG
		std::lock_guard<std::mutex> lock(mtx);
		SetConsoleTextAttribute(Console, static_cast<WORD>(ConsoleColor::Pink));
		std::cout << Name << DebugString << log << EndLine;
#endif
	}

	void Logger::Warning(const std::string& log) const
	{
		std::lock_guard<std::mutex> lock(mtx);
		SetConsoleTextAttribute(Console, static_cast<WORD>(ConsoleColor::Yellow));
		std::cout << Name << WarningString << log << EndLine;
	}

	void Logger::Information(const std::string& log) const
	{
		std::lock_guard<std::mutex> lock(mtx);
		SetConsoleTextAttribute(Console, static_cast<WORD>(ConsoleColor::Green));
		std::cout << Name << InformationString << log << EndLine;
	}

	void Logger::Error(const std::string& log) const
	{
		std::lock_guard<std::mutex> lock(mtx);
		SetConsoleTextAttribute(Console, static_cast<WORD>(ConsoleColor::Red));
		std::cout << Name << ErrorString << log << EndLine;
	}
}