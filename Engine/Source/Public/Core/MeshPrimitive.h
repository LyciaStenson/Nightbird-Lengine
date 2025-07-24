#pragma once

#include <vector>
#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanTexture;
	class VulkanBuffer;
	class VulkanUniformBuffer;
	struct Vertex;
	
	enum class AlphaMode
	{
		Opaque, Mask, Blend
	};

	struct MeshPrimitiveInfo
	{
		std::vector<Vertex> vertices;
		std::vector<uint16_t> indices;

		glm::vec4 baseColorFactor;
		float metallicFactor;
		float roughnessFactor;

		bool hasBaseColorTexture = false;
		bool hasMetallicRoughnessTexture = false;
		bool hasNormalTexture = false;

		size_t baseColorTextureIndex;
		size_t metallicRoughnessTextureIndex;
		size_t normalTextureIndex;

		std::shared_ptr<VulkanTexture> baseColorTexture;
		std::shared_ptr<VulkanTexture> metallicRoughnessTexture;
		std::shared_ptr<VulkanTexture> normalTexture;

		bool doubleSided = false;
		
		AlphaMode alphaMode = AlphaMode::Opaque;
		float alphaCutoff = 0.5f;
	};

	class MeshPrimitive
	{
	public:
		MeshPrimitive(VulkanDevice* device, VkDescriptorSetLayout materialDescriptorSetLayout, VkDescriptorPool descriptorPool, const MeshPrimitiveInfo& info);
		~MeshPrimitive();
		
		const size_t GetIndicesSize() const;

		VkDescriptorImageInfo GetBaseColorInfo() const;
		VkDescriptorImageInfo GetMetallicRoughnessInfo() const;
		VkDescriptorImageInfo GetNormalInfo() const;
		
		const std::vector<VkDescriptorSet>& GetMaterialDescriptorSets() const;
		
		bool GetDoubleSided() const;
		AlphaMode GetAlphaMode() const;
		
		VulkanBuffer* vertexBuffer;
		VulkanBuffer* indexBuffer;

		glm::vec4 baseColorFactor;
		float metallicFactor;
		float roughnessFactor;

		std::shared_ptr<VulkanTexture> baseColorTexture;
		std::shared_ptr<VulkanTexture> metallicRoughnessTexture;
		std::shared_ptr<VulkanTexture> normalTexture;

	private:
		VulkanDevice* device;
		
		bool doubleSided = false;

		AlphaMode alphaMode = AlphaMode::Opaque;
		float alphaCutoff;

		size_t indicesSize;

		VkDescriptorSetLayout materialDescriptorSetLayout;

		std::vector<VkDescriptorSet> materialDescriptorSets;
		
		VulkanUniformBuffer* materialFactorsUniformBuffer;
		
		void CreateMaterialFactorsUniformBuffer();
		
		void CreateVertexBuffer(const std::vector<Vertex>& vertices);
		void CreateIndexBuffer(const std::vector<uint16_t>& indices);
		
		void CreateMaterialDescriptorSets(VkDescriptorPool descriptorPool);
	};
}