#include "MeshInstance.h"

#include "DescriptorSetLayoutManager.h"
#include "Device.h"
#include "Config.h"

#include "Core/Log.h"

namespace Nightbird::Vulkan
{
	struct alignas(16) MeshUBO
	{
		alignas(16) glm::mat4 model;
	};

	MeshInstance::MeshInstance(Device* device, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager)
	{
		CreateUniformBuffers(device);
		CreateDescriptorSets(device, descriptorPool, descriptorSetLayoutManager);
	}

	void MeshInstance::CreateUniformBuffers(Device* device)
	{
		m_UniformBuffers.reserve(Config::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_UniformBuffers.emplace_back(device, sizeof(MeshUBO), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	void MeshInstance::CreateDescriptorSets(Device* device, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager)
	{
		std::vector<VkDescriptorSetLayout> layouts(Config::MAX_FRAMES_IN_FLIGHT, descriptorSetLayoutManager->GetMeshDescriptorSetLayout());

		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());
		allocInfo.pSetLayouts = layouts.data();

		// Allocate a descriptor set for each frame in flight
		m_DescriptorSets.resize(Config::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(device->GetLogical(), &allocInfo, m_DescriptorSets.data()) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to allocate mesh instance descriptor sets");
			return;
		}

		// Update the descriptor set for each frame in flight
		for (size_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_UniformBuffers[i].Get();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(MeshUBO);

			VkWriteDescriptorSet descriptorWrite{};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = m_DescriptorSets[i];
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;
			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;

			vkUpdateDescriptorSets(device->GetLogical(), 1, &descriptorWrite, 0, nullptr);
		}
	}
	
	void MeshInstance::UpdateTransform(const glm::mat4& transform, uint32_t currentFrame)
	{
		MeshUBO meshUBO{};
		meshUBO.model = transform;
		
		memcpy(m_UniformBuffers[currentFrame].GetMappedData(), &meshUBO, sizeof(meshUBO));
	}

	const std::vector<VkDescriptorSet>& MeshInstance::GetDescriptorSets() const
	{
		return m_DescriptorSets;
	}
}