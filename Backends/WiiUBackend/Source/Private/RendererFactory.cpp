#include "Core/RendererFactory.h"

#include "Renderer.h"

namespace Nightbird
{
	std::unique_ptr<Nightbird::Renderer> CreateRenderer()
	{
		return std::make_unique<WiiU::Renderer>();
	}
}