#pragma once

#include "Core/Renderer.h"

#include <memory>

namespace Nightbird::Core
{
	std::unique_ptr<Renderer> CreateRenderer();
}