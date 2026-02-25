#include "Core/PlatformFactory.h"

#include "WiiUPlatform.h"

namespace Nightbird
{
	std::unique_ptr<Platform> CreatePlatform()
	{
		return std::make_unique<WiiUPlatform>();
	}
}