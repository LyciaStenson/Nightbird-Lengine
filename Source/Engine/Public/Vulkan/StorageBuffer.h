#pragma once

#include <volk.h>

namespace Nightbird
{
	class VulkanBuffer;
	class VulkanDevice;

	class VulkanStorageBuffer
	{
	public:
		VulkanStorageBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~VulkanStorageBuffer();

		VkBuffer Get() const;
		void* GetMappedData() const;

		void UploadData(const void* data, VkDeviceSize size);

	private:
		VulkanBuffer* buffer;
		VulkanDevice* device;

		void* mappedData = nullptr;
	};
}