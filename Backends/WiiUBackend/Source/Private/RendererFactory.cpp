#include "Core/RendererFactory.h"

#include "WiiURenderer.h"

namespace Nightbird
{
	std::unique_ptr<Renderer> CreateRenderer()
	{
		return std::make_unique<WiiURenderer>();
	}
}