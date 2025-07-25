#include <Vulkan/UniformBuffer.h>

#include <iostream>

#include <Vulkan/Device.h>

namespace Nightbird
{
	VulkanUniformBuffer::VulkanUniformBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags)
		: device(device)
	{
		buffer = new VulkanBuffer(device, size, usageFlags, propertyFlags);
		mappedData = buffer->Map();
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		if (mappedData)
			vmaUnmapMemory(device->GetAllocator(), buffer->GetAllocation());

		delete buffer;
	}

	VkBuffer VulkanUniformBuffer::Get() const
	{
		return buffer->Get();
	}

	void* VulkanUniformBuffer::GetMappedData() const
	{
		return mappedData;
	}
}