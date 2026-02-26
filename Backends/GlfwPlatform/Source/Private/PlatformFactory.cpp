#include "Core/PlatformFactory.h"

#include "Platform.h"

namespace Nightbird
{
	std::unique_ptr<Nightbird::Platform> CreatePlatform()
	{
		return std::make_unique<Glfw::Platform>();
	}
}