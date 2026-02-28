#pragma once

#include "UniformBuffer.h"

#include <volk.h>
#include <glm/glm.hpp>

#include <vector>

namespace Nightbird::Vulkan
{
	class Device;
	class DescriptorSetLayoutManager;

	class MeshInstance
	{
	public:
		MeshInstance(Device* device, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager);
		~MeshInstance() = default;

		void UpdateTransform(const glm::mat4& transform, uint32_t currentFrame);

		const std::vector<VkDescriptorSet>& GetDescriptorSets() const;

	private:
		std::vector<UniformBuffer> m_UniformBuffers;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		void CreateUniformBuffers(Device* device);
		void CreateDescriptorSets(Device* device, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager);
	};
}