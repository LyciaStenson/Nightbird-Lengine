#include "Vulkan/DescriptorSetLayoutManager.h"

#include "Vulkan/Device.h"

#include <iostream>
#include <array>

namespace Nightbird::Vulkan
{
	DescriptorSetLayoutManager::DescriptorSetLayoutManager(Device* device)
		: m_Device(device)
	{
		CreateGlobalDescriptorSetLayout();
		CreateMeshDescriptorSetLayout();
		CreateMaterialDescriptorSetLayout();
	}

	DescriptorSetLayoutManager::~DescriptorSetLayoutManager()
	{
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_GlobalDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_MeshDescriptorSetLayout, nullptr);
		vkDestroyDescriptorSetLayout(m_Device->GetLogical(), m_MaterialDescriptorSetLayout, nullptr);
	}

	VkDescriptorSetLayout DescriptorSetLayoutManager::GetGlobalDescriptorSetLayout() const
	{
		return m_GlobalDescriptorSetLayout;
	}

	VkDescriptorSetLayout DescriptorSetLayoutManager::GetMeshDescriptorSetLayout() const
	{
		return m_MeshDescriptorSetLayout;
	}

	VkDescriptorSetLayout DescriptorSetLayoutManager::GetMaterialDescriptorSetLayout() const
	{
		return m_MaterialDescriptorSetLayout;
	}

	void DescriptorSetLayoutManager::CreateGlobalDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding cameraBinding{};
		cameraBinding.binding = 0;
		cameraBinding.descriptorCount = 1;
		cameraBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		cameraBinding.pImmutableSamplers = nullptr;
		cameraBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding directionalLightsBinding{};
		directionalLightsBinding.binding = 1;
		directionalLightsBinding.descriptorCount = 1;
		directionalLightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		directionalLightsBinding.pImmutableSamplers = nullptr;
		directionalLightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding directionalLightsMetaBinding{};
		directionalLightsMetaBinding.binding = 2;
		directionalLightsMetaBinding.descriptorCount = 1;
		directionalLightsMetaBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		directionalLightsMetaBinding.pImmutableSamplers = nullptr;
		directionalLightsMetaBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding pointLightsBinding{};
		pointLightsBinding.binding = 3;
		pointLightsBinding.descriptorCount = 1;
		pointLightsBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		pointLightsBinding.pImmutableSamplers = nullptr;
		pointLightsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding pointLightsMetaBinding{};
		pointLightsMetaBinding.binding = 4;
		pointLightsMetaBinding.descriptorCount = 1;
		pointLightsMetaBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pointLightsMetaBinding.pImmutableSamplers = nullptr;
		pointLightsMetaBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 5> bindings = {cameraBinding, directionalLightsBinding, directionalLightsMetaBinding, pointLightsBinding, pointLightsMetaBinding};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_GlobalDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create global descriptor set layout" << std::endl;
		}
	}

	void DescriptorSetLayoutManager::CreateMeshDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding uboBinding{};
		uboBinding.binding = 0;
		uboBinding.descriptorCount = 1;
		uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboBinding.pImmutableSamplers = nullptr;
		uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 1;
		layoutInfo.pBindings = &uboBinding;

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_MeshDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create mesh descriptor set layout" << std::endl;
		}
	}

	void DescriptorSetLayoutManager::CreateMaterialDescriptorSetLayout()
	{
		VkDescriptorSetLayoutBinding factorsBinding{};
		factorsBinding.binding = 0;
		factorsBinding.descriptorCount = 1;
		factorsBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		factorsBinding.pImmutableSamplers = nullptr;
		factorsBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding baseColorBinding{};
		baseColorBinding.binding = 1;
		baseColorBinding.descriptorCount = 1;
		baseColorBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		baseColorBinding.pImmutableSamplers = nullptr;
		baseColorBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding metallicRoughnessBinding{};
		metallicRoughnessBinding.binding = 2;
		metallicRoughnessBinding.descriptorCount = 1;
		metallicRoughnessBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		metallicRoughnessBinding.pImmutableSamplers = nullptr;
		metallicRoughnessBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding normalBinding{};
		normalBinding.binding = 3;
		normalBinding.descriptorCount = 1;
		normalBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		normalBinding.pImmutableSamplers = nullptr;
		normalBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 4> bindings = {factorsBinding, baseColorBinding, metallicRoughnessBinding, normalBinding};
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(m_Device->GetLogical(), &layoutInfo, nullptr, &m_MaterialDescriptorSetLayout) != VK_SUCCESS)
		{
			std::cerr << "Failed to create mesh descriptor set layout" << std::endl;
		}
	}
}