#pragma once

#include "Vulkan/Image.h"

#include <volk.h>

#include <memory>

namespace Nightbird::Core
{
	class Texture;
}

namespace Nightbird::Vulkan
{
	class Device;

	class Texture
	{
	public:
		// Create from CPU texture data
		Texture(Device* device, const Core::Texture& texture, bool sRGB = true);
		// Create for render target
		Texture(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkImageAspectFlags aspectFlags);
		~Texture();

		VkImageView GetImageView() const;
		VkSampler GetSampler() const;

		void TransitionToShaderRead(VkCommandBuffer commandBuffer);
		void TransitionToColor(VkCommandBuffer commandBuffer);

		void SetImageLayout(VkImageLayout layout);

	private:
		std::unique_ptr<Image> m_Image;

		VkSampler m_Sampler = VK_NULL_HANDLE;

		Device* m_Device;

		void CreateFromPixels(const uint8_t* pixels, uint32_t width, uint32_t height, bool sRGB);
		void CreateTextureSampler();
	};
}