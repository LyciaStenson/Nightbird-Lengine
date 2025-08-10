#pragma once

#include <vector>
#include <optional>

#include <GLFW/glfw3.h>

#include <volk.h>
#include <vk_mem_alloc.h>

#include <Vulkan/Helpers.h>

namespace Nightbird
{
	class VulkanDevice
	{
	public:
		VulkanDevice(VkInstance instance, VkSurfaceKHR surface);
		~VulkanDevice();

		uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags);

		VkCommandBuffer BeginSingleTimeCommands() const;
		void EndSingleTimeCommands(VkCommandBuffer commandBuffer) const;

		VkDevice GetLogical() const;
		VkPhysicalDevice GetPhysical() const;

		VmaAllocator GetAllocator() const;

		VkPhysicalDeviceProperties GetPhysicalDeviceProperties() const;

		std::vector<VkCommandBuffer> commandBuffers;

		VkQueue graphicsQueue;
		VkQueue presentQueue;

		uint32_t graphicsQueueFamily;
		uint32_t presentQueueFamily;

	private:
		VkDevice logicalDevice;
		VkPhysicalDevice physicalDevice;
		VkPhysicalDeviceProperties physicalDeviceProperties;

		VkInstance instance;
		VkSurfaceKHR surface;

		VmaAllocator allocator;

		VkCommandPool commandPool;
		
		bool CheckDeviceExtensionSupport();

		void SelectPhysicalDevice();
		void CreateLogicalDevice();

		void CreateAllocator();

		void CreateCommandPool();
		void CreateCommandBuffers();
	};
}