#include "Vulkan/SwapChainSurface.h"

#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"

#include "Core/Log.h"

namespace Nightbird::Vulkan
{
	SwapChainSurface::SwapChainSurface(Device& device, SwapChain& swapChain, RenderPass& renderPass)
		: m_Device(device), m_SwapChain(swapChain), m_RenderPass(renderPass)
	{

	}

	uint32_t SwapChainSurface::GetWidth() const
	{
		return m_SwapChain.m_Extent.width;
	}

	uint32_t SwapChainSurface::GetHeight() const
	{
		return m_SwapChain.m_Extent.height;
	}

	VkExtent2D SwapChainSurface::GetExtent() const
	{
		return m_SwapChain.m_Extent;
	}

	VkFramebuffer SwapChainSurface::AcquireFramebuffer(VkSemaphore imageAvailableSemaphore)
	{
		VkResult result = vkAcquireNextImageKHR(
			m_Device.GetLogical(), m_SwapChain.Get(),
			UINT64_MAX,
			imageAvailableSemaphore,
			VK_NULL_HANDLE,
			&m_CurrentImageIndex
		);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			// Should recreate swap chain
			Core::Log::Warning("Swap chain out of date when acquiring framebuffer");
			return VK_NULL_HANDLE;
		}
		else if (result != VK_SUCCESS)
		{
			Core::Log::Error("Failed to acquire swap chain image");
			return VK_NULL_HANDLE;
		}

		return m_SwapChain.m_Framebuffers[m_CurrentImageIndex];
	}

	void SwapChainSurface::Present(VkSemaphore renderFinishedSemaphore)
	{
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;

		VkSwapchainKHR swapChains[] = { m_SwapChain.Get() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_CurrentImageIndex;

		VkResult result = vkQueuePresentKHR(m_Device.GetPresentQueue(), &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			// Should recreate swap chain
			Core::Log::Warning("Swap chain out of date when presenting");
			return;
		}
		else if (result != VK_SUCCESS)
		{
			Core::Log::Error("Failed to present swap chain image");
		}
	}

	VkRenderPass SwapChainSurface::GetCompatibleRenderPass() const
	{
		return m_RenderPass.Get();
	}
}
