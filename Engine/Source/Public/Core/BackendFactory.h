#pragma once

#include "Core/Platform.h"
#include "Core/Renderer.h"

#include <memory>

namespace Nightbird::Core
{
	std::unique_ptr<Renderer> CreateRenderer();
	std::unique_ptr<Platform> CreatePlatform();
}