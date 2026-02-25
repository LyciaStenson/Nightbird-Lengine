#pragma once

#include "Core/Platform.h"

#include <memory>

namespace Nightbird
{
	std::unique_ptr<Platform> CreatePlatform();
}