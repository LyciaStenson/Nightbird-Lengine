#include "Core/Log.h"

#include <whb/log.h>

namespace Nightbird
{
	void Core::Log::Info(const std::string& message)
	{
		WHBLogPrintf("[Info] %s", message.c_str());
	}

	void Core::Log::Warning(const std::string& message)
	{
		WHBLogPrintf("[Warning] %s", message.c_str());
	}

	void Core::Log::Error(const std::string& message)
	{
		WHBLogPrintf("[Error] %s", message.c_str());
	}
}