#include "Core/Log.h"

#include <iostream>

namespace Nightbird::Core
{
	Log::LogFunc Log::s_InfoFunc = [](const std::string& message) { std::cout << "[Info] " << message << std::endl; };
	Log::LogFunc Log::s_WarningFunc = [](const std::string& message) { std::cout << "[Warning] " << message << std::endl; };
	Log::LogFunc Log::s_ErrorFunc = [](const std::string& message) { std::cerr << "[Error] " << message << std::endl; };

	void Log::Info(const std::string& message)
	{
		s_InfoFunc(message);
	}

	void Log::Warning(const std::string& message)
	{
		s_WarningFunc(message);
	}

	void Log::Error(const std::string& message)
	{
		s_ErrorFunc(message);
	}

	void Log::SetInfoFunc(LogFunc func)
	{
		s_InfoFunc = func;
	}

	void Log::SetWarningFunc(LogFunc func)
	{
		s_WarningFunc = func;
	}

	void Log::SetErrorFunc(LogFunc func)
	{
		s_ErrorFunc = func;
	}
}
