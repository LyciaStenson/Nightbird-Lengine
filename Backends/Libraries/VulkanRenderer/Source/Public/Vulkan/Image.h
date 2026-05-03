#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Device;

	class Image
	{
	public:
		Image(Device* device, uint32_t width, uint32_t height, VkFormat format, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImageAspectFlags aspectFlags);
		Image(Device* device, VkImage existingImage, VkFormat format, VkImageAspectFlags aspectFlags);
		~Image();

		VkImage Get() const;
		VkImageView GetImageView() const;

		void CreateImageView(VkImageAspectFlags aspectFlags);

		void TransitionImageLayout(VkImageLayout newLayout);
		void TransitionImageLayout(VkCommandBuffer commandBuffer, VkImageLayout newLayout);

		void SetLayout(VkImageLayout layout);

	private:
		VkImage image;
		VkImageLayout currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		VkFormat format;

		VkImageView imageView;
		VkDeviceMemory memory;

		Device* device;

		bool ownsImage;

		void CreateImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkImage& image, VkDeviceMemory& imageMemory);
	};
}