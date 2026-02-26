#pragma once

#include <vector>
#include <memory>

#include <volk.h>

namespace Nightbird
{
	struct Renderable;
	class Mesh;
	class Camera;
}

namespace Nightbird::Vulkan
{
	class Device;
	class RenderPass;
	class DescriptorSetLayoutManager;
	class GlobalDescriptorSetManager;

	enum class PipelineType
	{
		Opaque,
		Transparent
	};

	class Pipeline
	{
	public:
		Pipeline(Device* device, RenderPass* renderPass, DescriptorSetLayoutManager* descriptorSetLayoutManager, GlobalDescriptorSetManager* globalDescriptorSetManager, PipelineType type, bool doubleSided);
		~Pipeline();

		void SetDescriptorPool(VkDescriptorPool pool);

		void Render(VkCommandBuffer commandBuffer, uint32_t currentFrame, const std::vector<Renderable>& renderables, Camera* camera);
		void RenderSingle(VkCommandBuffer commandBuffer, uint32_t currentFrame, const Renderable& renderable, Camera* camera);

	private:
		void CreateGraphicsPipeline(DescriptorSetLayoutManager* layoutManager);

		VkPipeline pipeline;
		VkPipelineLayout pipelineLayout;

		VkDescriptorPool descriptorPool;

		PipelineType type;
		bool doubleSided;

		RenderPass* renderPass;

		GlobalDescriptorSetManager* globalDescriptorSetManager;

		Device* device;
	};
}