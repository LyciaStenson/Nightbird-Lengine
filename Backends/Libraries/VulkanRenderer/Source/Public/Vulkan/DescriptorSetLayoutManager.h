#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Device;

	class DescriptorSetLayoutManager
	{
	public:
		DescriptorSetLayoutManager(Device* device);
		~DescriptorSetLayoutManager();

		VkDescriptorSetLayout GetGlobalDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMeshDescriptorSetLayout() const;
		VkDescriptorSetLayout GetMaterialDescriptorSetLayout() const;

	private:
		void CreateGlobalDescriptorSetLayout();
		void CreateMeshDescriptorSetLayout();
		void CreateMaterialDescriptorSetLayout();

		VkDescriptorSetLayout m_GlobalDescriptorSetLayout;
		VkDescriptorSetLayout m_MeshDescriptorSetLayout;
		VkDescriptorSetLayout m_MaterialDescriptorSetLayout;

		Device* m_Device;
	};
}