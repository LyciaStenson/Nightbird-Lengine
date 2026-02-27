#include "Core/Log.h"

#include <iostream>

namespace Nightbird
{
	void Log::Info(const std::string& message)
	{
		std::cout << "[Info] " << message << std::endl;
	}

	void Log::Warning(const std::string& message)
	{
		std::cout << "[Warning] " << message << std::endl;
	}
	
	void Log::Error(const std::string& message)
	{
		std::cerr << "[Error] " << message << std::endl;
	}
}