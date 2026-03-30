#pragma once

#include "Core/RenderSurface.h"

#include <volk.h>

#include <cstdint>

namespace Nightbird::Vulkan
{
	class RenderSurface : public Core::RenderSurface
	{
	public:
		virtual VkFramebuffer AcquireFramebuffer(VkSemaphore imageAvailableSemaphore) = 0;
		virtual void Present(VkSemaphore renderFinishedSemaphore) = 0;
		virtual VkExtent2D GetExtent() const = 0;
		virtual VkRenderPass GetCompatibleRenderPass() const = 0;
		virtual bool NeedsPresent() const = 0;
	};
}
