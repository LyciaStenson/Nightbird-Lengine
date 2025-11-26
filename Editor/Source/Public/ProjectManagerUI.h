#pragma once

#include "UIState.h"

#include "volk.h"

namespace Nightbird
{
	class Renderer;
	class VulkanRenderPass;

	class ProjectManagerUI : public UIState
	{
	public:
		virtual void Render(Renderer* renderer, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;

		virtual bool ShouldClose() override;

		std::string projectPath = "";

	private:
		virtual void NewFrame() override;
		virtual void Draw(VkCommandBuffer commandBuffer) override;

		bool m_ShouldClose = false;
	};
}