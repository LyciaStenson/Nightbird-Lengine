#pragma once

#include <volk.h>

#include <memory>
#include <vector>

namespace Nightbird::Vulkan
{
	class Device;
	class Image;

	class SwapChain
	{
	public:
		SwapChain(Device* device, VkSurfaceKHR surface);
		~SwapChain();

		VkSwapchainKHR Get() const;

		uint32_t GetImageCount() const;
		uint32_t GetMinImageCount() const;

		VkFormat GetColorFormat() const;
		VkFormat GetDepthFormat() const;

		void CreateSwapChain();
		void CreateDepthResources();
		void CreateFramebuffers(VkRenderPass renderPass);

		void CleanupSwapChain();

		std::vector<VkFramebuffer> m_Framebuffers;
		VkExtent2D m_Extent;

		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;

	private:
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		VkFormat m_ColorFormat;
		VkFormat m_DepthFormat;

		std::vector<std::unique_ptr<Image>> m_Images;

		VkSurfaceKHR m_Surface;

		std::unique_ptr<Image> m_DepthImage;

		Device* m_Device;
		
		uint32_t m_ImageCount;
		uint32_t m_MinImageCount;
	};
}
