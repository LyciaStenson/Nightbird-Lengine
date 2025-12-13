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

	private:
		virtual void NewFrame() override;
		virtual void Draw(VkCommandBuffer commandBuffer) override;

		void CreateProject(const std::string& path, const std::string& name);

		Engine* m_Engine = nullptr;

		EditorRenderTarget* m_RenderTarget = nullptr;

		bool m_ShouldClose = false;

		std::string m_ProjectPath;
		std::string m_ProjectName;
	};
}