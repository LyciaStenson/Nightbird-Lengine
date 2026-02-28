#include "Core/PlatformFactory.h"

#include "Platform.h"

namespace Nightbird::Core
{
	std::unique_ptr<Core::Platform> CreatePlatform()
	{
		return std::make_unique<WiiU::Platform>();
	}
}