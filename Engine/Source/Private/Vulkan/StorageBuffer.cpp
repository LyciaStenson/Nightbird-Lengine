#include <Vulkan/StorageBuffer.h>

#include <Vulkan/Buffer.h>
#include <Vulkan/Device.h>

#include <iostream>

namespace Nightbird
{
	VulkanStorageBuffer::VulkanStorageBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
		: device(device)
	{
		buffer = new Nightbird::VulkanBuffer(device, size, usageFlags | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, propertyFlags);

		if (propertyFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
			mappedData = buffer->Map();
	}

	VulkanStorageBuffer::~VulkanStorageBuffer()
	{
		if (mappedData)
			vmaUnmapMemory(device->GetAllocator(), buffer->GetAllocation());

		delete buffer;
	}

	VkBuffer VulkanStorageBuffer::Get() const
	{
		return buffer->Get();
	}

	void* VulkanStorageBuffer::GetMappedData() const
	{
		return mappedData;
	}

	void VulkanStorageBuffer::UploadData(const void* data, VkDeviceSize size)
	{
		if (!mappedData)
			return;

		std::memcpy(mappedData, data, static_cast<size_t>(size));
	}
}