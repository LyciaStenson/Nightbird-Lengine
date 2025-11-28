#pragma once

#include "UIState.h"

#include "volk.h"

namespace Nightbird
{
	class Engine;
	class EditorRenderTarget;
	class Renderer;
	class VulkanRenderPass;

	class ProjectManagerUI : public UIState
	{
	public:
		void Init(Engine* engine, EditorRenderTarget* renderTarget);

		virtual void Render(Renderer* renderer, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;
		
		std::string projectPath = "";

	private:
		virtual void NewFrame() override;
		virtual void Draw(VkCommandBuffer commandBuffer) override;

		Engine* m_Engine = nullptr;

		EditorRenderTarget* m_RenderTarget = nullptr;

		bool m_ShouldClose = false;
	};
}