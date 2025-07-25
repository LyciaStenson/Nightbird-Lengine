#pragma once

#include <memory>

#include <volk.h>

#include <Vulkan/Buffer.h>

namespace Nightbird
{
	class VulkanDevice;

	class VulkanUniformBuffer
	{
	public:
		VulkanUniformBuffer(VulkanDevice* device, VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags);
		~VulkanUniformBuffer();

		VkBuffer Get() const;
		
		void* GetMappedData() const;

	private:
		VulkanBuffer* buffer;

		VulkanDevice* device;

		void* mappedData = nullptr;
	};
}