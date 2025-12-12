#include <Vulkan/GlobalDescriptorSetManager.h>

#include <array>
#include <cstring>
#include <iostream>

#include <Vulkan/Config.h>
#include <Vulkan/Device.h>
#include <Vulkan/UniformBuffer.h>
#include <Vulkan/StorageBuffer.h>
#include <Core/CameraUBO.h>
#include <Core/DirectionalLightData.h>
#include <Core/PointLightData.h>

namespace Nightbird
{
	GlobalDescriptorSetManager::GlobalDescriptorSetManager(VulkanDevice* device, VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
		: device(device)
	{
		CreateBuffers();
		CreateDescriptorSets(descriptorSetLayout, descriptorPool);
	}

	GlobalDescriptorSetManager::~GlobalDescriptorSetManager()
	{

	}

	const std::vector<VkDescriptorSet>& GlobalDescriptorSetManager::GetDescriptorSets() const
	{
		return descriptorSets;
	}

	void GlobalDescriptorSetManager::UpdateCamera(uint32_t frameIndex, const CameraUBO& cameraUBO)
	{
		memcpy(cameraBuffers[frameIndex].GetMappedData(), &cameraUBO, sizeof(cameraUBO));
	}

	void GlobalDescriptorSetManager::UpdateDirectionalLights(uint32_t frameIndex, const std::vector<DirectionalLightData>& directionalLights)
	{
		VkDeviceSize size = sizeof(PointLightData) * directionalLights.size();
		if (size > 0)
		{
			directionalLightBuffers[frameIndex].UploadData(directionalLights.data(), size);

			DirectionalLightMetaUBO metaUBO{};
			metaUBO.count = static_cast<uint32_t>(directionalLights.size());
			memcpy(directionalLightMetaBuffers[frameIndex].GetMappedData(), &metaUBO, sizeof(metaUBO));
		}
	}

	void GlobalDescriptorSetManager::UpdatePointLights(uint32_t frameIndex, const std::vector<PointLightData>& pointLights)
	{
		VkDeviceSize size = sizeof(PointLightData) * pointLights.size();
		if (size > 0)
		{
			pointLightBuffers[frameIndex].UploadData(pointLights.data(), size);

			PointLightMetaUBO metaUBO{};
			metaUBO.count = static_cast<uint32_t>(pointLights.size());
			memcpy(pointLightMetaBuffers[frameIndex].GetMappedData(), &metaUBO, sizeof(metaUBO));
		}
	}

	void GlobalDescriptorSetManager::CreateBuffers()
	{
		VkDeviceSize cameraSize = sizeof(CameraUBO);
		VkDeviceSize directionalLightBufferSize = sizeof(DirectionalLightData) * MAX_DIRECTIONAL_LIGHTS;
		VkDeviceSize directionalLightMetaBufferSize = sizeof(DirectionalLightMetaUBO) * MAX_DIRECTIONAL_LIGHTS;
		VkDeviceSize pointLightBufferSize = sizeof(PointLightData) * MAX_POINT_LIGHTS;
		VkDeviceSize pointLightMetaBufferSize = sizeof(PointLightMetaUBO) * MAX_POINT_LIGHTS;

		cameraBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
		directionalLightBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
		directionalLightMetaBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
		pointLightBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
		pointLightMetaBuffers.reserve(VulkanConfig::MAX_FRAMES_IN_FLIGHT);

		for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
		{
			cameraBuffers.emplace_back(device, cameraSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			directionalLightBuffers.emplace_back(device, directionalLightBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			directionalLightMetaBuffers.emplace_back(device, directionalLightMetaBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			pointLightBuffers.emplace_back(device, pointLightBufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			pointLightMetaBuffers.emplace_back(device, pointLightMetaBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	void GlobalDescriptorSetManager::CreateDescriptorSets(VkDescriptorSetLayout descriptorSetLayout, VkDescriptorPool descriptorPool)
	{
		VkDevice logicalDevice = device->GetLogical();

		std::vector<VkDescriptorSetLayout> layouts(VulkanConfig::MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = descriptorPool;
		allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
		allocInfo.pSetLayouts = layouts.data();

		descriptorSets.resize(VulkanConfig::MAX_FRAMES_IN_FLIGHT);
		if (vkAllocateDescriptorSets(logicalDevice, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
		{
			std::cerr << "Failed to allocate global descriptor sets" << std::endl;
			return;
		}

		for (size_t i = 0; i < VulkanConfig::MAX_FRAMES_IN_FLIGHT; i++)
		{
			VkDescriptorBufferInfo cameraInfo{};
			cameraInfo.buffer = cameraBuffers[i].Get();
			cameraInfo.offset = 0;
			cameraInfo.range = sizeof(CameraUBO);

			VkDescriptorBufferInfo directionalLightsInfo{};
			directionalLightsInfo.buffer = directionalLightBuffers[i].Get();
			directionalLightsInfo.offset = 0;
			directionalLightsInfo.range = VK_WHOLE_SIZE;
			//directionalLightsInfo.range = sizeof(PointLightData) * MAX_DIRECTIONAL_LIGHTS;

			VkDescriptorBufferInfo directionalLightsMetaInfo{};
			directionalLightsMetaInfo.buffer = directionalLightMetaBuffers[i].Get();
			directionalLightsMetaInfo.offset = 0;
			directionalLightsMetaInfo.range = sizeof(DirectionalLightMetaUBO);

			VkDescriptorBufferInfo pointLightsInfo{};
			pointLightsInfo.buffer = pointLightBuffers[i].Get();
			pointLightsInfo.offset = 0;
			pointLightsInfo.range = VK_WHOLE_SIZE;
			//pointLightsInfo.range = sizeof(PointLightData) * MAX_POINT_LIGHTS;

			VkDescriptorBufferInfo pointLightsMetaInfo{};
			pointLightsMetaInfo.buffer = pointLightMetaBuffers[i].Get();
			pointLightsMetaInfo.offset = 0;
			pointLightsMetaInfo.range = sizeof(PointLightMetaUBO);

			std::array<VkWriteDescriptorSet, 5> descriptorWrites{};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = descriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &cameraInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = descriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &directionalLightsInfo;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].dstSet = descriptorSets[i];
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = &directionalLightsMetaInfo;

			descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[3].dstSet = descriptorSets[i];
			descriptorWrites[3].dstBinding = 3;
			descriptorWrites[3].dstArrayElement = 0;
			descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			descriptorWrites[3].descriptorCount = 1;
			descriptorWrites[3].pBufferInfo = &pointLightsInfo;

			descriptorWrites[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[4].dstSet = descriptorSets[i];
			descriptorWrites[4].dstBinding = 4;
			descriptorWrites[4].dstArrayElement = 0;
			descriptorWrites[4].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[4].descriptorCount = 1;
			descriptorWrites[4].pBufferInfo = &pointLightsMetaInfo;

			vkUpdateDescriptorSets(logicalDevice, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}
}
