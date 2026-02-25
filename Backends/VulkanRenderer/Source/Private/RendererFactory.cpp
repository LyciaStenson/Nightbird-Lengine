#include "Core/RendererFactory.h"

#include "VulkanRenderer.h"

namespace Nightbird
{
	std::unique_ptr<Renderer> CreateRenderer()
	{
		return std::make_unique<VulkanRenderer>();
	}
}