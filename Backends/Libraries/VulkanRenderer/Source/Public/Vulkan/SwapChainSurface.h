#pragma once

#include "Vulkan/RenderSurface.h"

namespace Nightbird::Vulkan
{
	class Device;
	class SwapChain;
	class RenderPass;

	class SwapChainSurface : public RenderSurface
	{
	public:
		SwapChainSurface(Device& device, SwapChain& swapChain, RenderPass& renderPass);

		uint32_t GetWidth() const override;
		uint32_t GetHeight() const override;

		VkExtent2D GetExtent() const override;
		VkFramebuffer AcquireFramebuffer(VkSemaphore imageAvailableSemaphore) override;
		void Present(VkSemaphore renderFinishedSemaphore) override;
		VkRenderPass GetCompatibleRenderPass() const override;
		bool NeedsPresent() const override { return true; }

	private:
		Device& m_Device;
		SwapChain& m_SwapChain;
		RenderPass& m_RenderPass;

		uint32_t m_CurrentImageIndex = 0;
	};
}
