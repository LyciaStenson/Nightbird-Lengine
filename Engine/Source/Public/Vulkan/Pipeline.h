#pragma once

#include <vector>
#include <memory>

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanDescriptorSetLayoutManager;
	class GlobalDescriptorSetManager;
	struct Renderable;
	class Mesh;
	class Camera;
	
	enum class PipelineType
	{
		Opaque,
		Transparent
	};

	class VulkanPipeline
	{
	public:
		VulkanPipeline(VulkanDevice* device, VulkanRenderPass* renderPass, VulkanDescriptorSetLayoutManager* descriptorSetLayoutManager, GlobalDescriptorSetManager* globalDescriptorSetManager, PipelineType type, bool doubleSided);
		~VulkanPipeline();

		void SetDescriptorPool(VkDescriptorPool pool);
		
		void Render(VkCommandBuffer commandBuffer, uint32_t currentFrame, const std::vector<Renderable>& renderables, Camera* camera);

	private:
		void CreateGraphicsPipeline(VulkanDescriptorSetLayoutManager* layoutManager);

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;
		
		VkDescriptorPool descriptorPool;

		PipelineType type;
		bool doubleSided;
		
		VulkanRenderPass* renderPass;

		GlobalDescriptorSetManager* globalDescriptorSetManager;
		
		VulkanDevice* device;
	};
}