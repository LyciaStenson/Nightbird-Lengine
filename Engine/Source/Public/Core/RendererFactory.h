#pragma once

#include "Core/Platform.h"

#include <memory>

namespace Nightbird::Core
{
	std::unique_ptr<Platform> CreatePlatform();
}