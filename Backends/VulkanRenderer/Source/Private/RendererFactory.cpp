#include "Core/RendererFactory.h"

#include "Renderer.h"

namespace Nightbird::Core
{
	std::unique_ptr<Core::Renderer> CreateRenderer()
	{
		return std::make_unique<Vulkan::Renderer>();
	}
}