#pragma once

#include "Texture.h"
#include "UniformBuffer.h"

#include <volk.h>

#include <vector>
#include <memory>

namespace Nightbird::Core
{
	class Material;
}

namespace Nightbird::Vulkan
{
	class Device;
	class DescriptorSetLayoutManager;

	class Material
	{
	public:
		Material(Device* device, const Core::Material& material, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager);
		~Material() = default;

		const std::vector<VkDescriptorSet>& GetDescriptorSets() const;

	private:
		std::unique_ptr<Texture> m_BaseColorTexture;
		std::unique_ptr<Texture> m_MetallicRoughnessTexture;
		std::unique_ptr<Texture> m_NormalTexture;

		std::unique_ptr<UniformBuffer> m_FactorsUniformBuffer;

		std::vector<VkDescriptorSet> m_DescriptorSets;

		void CreateTextures(Device* device, const Core::Material& material);
		void CreateFactorsBuffer(Device* device, const Core::Material& material);
		void CreateDescriptorSets(Device* device, const Core::Material& material, VkDescriptorPool descriptorPool, DescriptorSetLayoutManager* descriptorSetLayoutManager);
	};
}