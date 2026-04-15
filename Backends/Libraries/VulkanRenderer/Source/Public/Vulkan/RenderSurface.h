#pragma once

#include "Core/RenderSurface.h"

#include <volk.h>

#include <cstdint>

namespace Nightbird::Vulkan
{
	class RenderPass;
	
	class RenderSurface : public Core::RenderSurface
	{
	public:
		//NB_OBJECT()
		virtual VkExtent2D GetExtent() const = 0;
		virtual RenderPass& GetRenderPass() const = 0;
		virtual bool NeedsResize() const = 0;
	};
}
