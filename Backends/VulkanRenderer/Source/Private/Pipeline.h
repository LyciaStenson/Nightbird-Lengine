#pragma once

#include <volk.h>

namespace Nightbird::Vulkan
{
	class Device;
	class RenderPass;
	class DescriptorSetLayoutManager;

	enum class PipelineType
	{
		Opaque,
		Transparent
	};

	class Pipeline
	{
	public:
		Pipeline(Device* device, RenderPass* renderPass, DescriptorSetLayoutManager* descriptorSetLayoutManager, PipelineType type, bool doubleSided);
		~Pipeline();

		void Bind(VkCommandBuffer commandBuffer) const;

		VkPipelineLayout GetLayout() const;

	private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;

		Device* m_Device;

		void CreateGraphicsPipeline(RenderPass* renderPass, DescriptorSetLayoutManager* descriptorSetLayoutManager, PipelineType type, bool doubleSided);
	};
}