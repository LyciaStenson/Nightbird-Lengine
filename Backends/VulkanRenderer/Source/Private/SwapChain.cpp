#include "SwapChain.h"

#include "Image.h"
#include "Device.h"
#include "ImageUtils.h"
#include "SwapChainUtils.h"

#include <iostream>
#include <algorithm>
#include <vector>
#include <array>
#include <limits>

namespace Nightbird::Vulkan
{
	SwapChain::SwapChain(Device* device, VkSurfaceKHR surface)
		: m_Device(device), m_Surface(surface)
	{
		CreateSwapChain();
		CreateDepthResources();
	}

	SwapChain::~SwapChain()
	{
		CleanupSwapChain();
	}

	VkSwapchainKHR SwapChain::Get() const
	{
		return m_SwapChain;
	}

	uint32_t SwapChain::GetImageCount() const
	{
		return m_ImageCount;
	}

	uint32_t SwapChain::GetMinImageCount() const
	{
		return m_MinImageCount;
	}

	VkFormat SwapChain::GetColorFormat() const
	{
		return m_ColorFormat;
	}

	VkFormat SwapChain::GetDepthFormat() const
	{
		return m_DepthFormat;
	}

	void SwapChain::CreateSwapChain()
	{
		VkDevice logicalDevice = m_Device->GetLogical();
		VkPhysicalDevice physicalDevice = m_Device->GetPhysical();

		SwapChainSupportDetails supportDetails = QuerySwapChainSupport(physicalDevice, m_Surface);

		m_SurfaceFormat = ChooseSwapSurfaceFormat(supportDetails.formats);
		m_PresentMode = ChooseSwapPresentMode(supportDetails.presentModes);

		m_Extent = ChooseSwapExtent(supportDetails.capabilities);

		m_MinImageCount = supportDetails.capabilities.minImageCount;
		m_ImageCount = m_MinImageCount + 1;
		if (supportDetails.capabilities.maxImageCount > 0 && m_ImageCount > supportDetails.capabilities.maxImageCount)
			m_ImageCount = supportDetails.capabilities.maxImageCount;

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;

		createInfo.minImageCount = m_ImageCount;
		createInfo.imageFormat = m_SurfaceFormat.format;
		createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		uint32_t graphicsFamily = m_Device->GetGraphicsQueueFamily();
		uint32_t presentFamily = m_Device->GetPresentQueueFamily();
		uint32_t queueFamilyIndices[] = { graphicsFamily, presentFamily };
		
		if (graphicsFamily != presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = supportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		{
			std::cerr << "Failed to create swap chain" << std::endl;
			return;
		}

		vkGetSwapchainImagesKHR(logicalDevice, m_SwapChain, &m_ImageCount, nullptr);

		std::vector<VkImage> rawImages(m_ImageCount);
		vkGetSwapchainImagesKHR(logicalDevice, m_SwapChain, &m_ImageCount, rawImages.data());

		m_Images.clear();
		for (VkImage image : rawImages)
		{
			m_Images.push_back(std::make_unique<Image>(m_Device, image, m_SurfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT));
		}

		m_ColorFormat = m_SurfaceFormat.format;
	}

	void SwapChain::CleanupSwapChain()
	{
		VkDevice logicalDevice = m_Device->GetLogical();
		
		for (VkFramebuffer framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		
		vkDestroySwapchainKHR(logicalDevice, m_SwapChain, nullptr);
	}

	void SwapChain::CreateDepthResources()
	{
		m_DepthFormat = FindDepthFormat(m_Device->GetPhysical());
		m_DepthImage = std::make_unique<Image>(m_Device, m_Extent.width, m_Extent.height, m_DepthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
		m_DepthImage->TransitionImageLayout(VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	}

	void SwapChain::CreateFramebuffers(VkRenderPass renderPass)
	{
		m_Framebuffers.resize(m_Images.size());

		for (size_t i = 0; i < m_Images.size(); i++)
		{
			std::array<VkImageView, 2> attachments =
			{
				m_Images[i]->GetImageView(),
				m_DepthImage->GetImageView()
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_Extent.width;
			framebufferInfo.height = m_Extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device->GetLogical(), &framebufferInfo, nullptr, &m_Framebuffers[i]) != VK_SUCCESS)
			{
				std::cerr << "Failed to create framebuffers" << std::endl;
			}
		}
	}

	VkSurfaceFormatKHR SwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR SwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D SwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		int width, height;
		width = 1280;
		height = 720;
		//glfwGetFramebufferSize(window, &width, &height);

		VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return actualExtent;
	}
}
