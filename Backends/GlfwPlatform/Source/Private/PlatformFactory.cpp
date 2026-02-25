#include "Core/PlatformFactory.h"

#include "GlfwPlatform.h"

namespace Nightbird
{
	std::unique_ptr<Platform> CreatePlatform()
	{
		return std::make_unique<GlfwPlatform>();
	}
}