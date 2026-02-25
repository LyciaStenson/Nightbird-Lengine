#pragma once

#include "Core/Renderer.h"

#include <memory>

namespace Nightbird
{
	std::unique_ptr<Renderer> CreateRenderer();
}