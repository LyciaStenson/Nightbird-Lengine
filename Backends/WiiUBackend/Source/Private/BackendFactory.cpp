#include "Core/BackendFactory.h"

#include "Platform.h"
#include "Renderer.h"

namespace Nightbird::Core
{
	std::unique_ptr<Core::Platform> CreatePlatform()
	{
		return std::make_unique<WiiU::Platform>();
	}

	std::unique_ptr<Core::Renderer> CreateRenderer()
	{
		return std::make_unique<WiiU::Renderer>();
	}
}