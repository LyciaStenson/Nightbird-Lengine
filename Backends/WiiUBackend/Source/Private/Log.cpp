#include "Core/Log.h"

#include <whb/log.h>

namespace Nightbird
{
	void Log::Info(const std::string& message)
	{
		WHBLogPrintf("[Info] %s", message.c_str());
	}

	void Log::Warning(const std::string& message)
	{
		WHBLogPrintf("[Warning] %s", message.c_str());
	}

	void Log::Error(const std::string& message)
	{
		WHBLogPrintf("[Error] %s", message.c_str());
	}
}