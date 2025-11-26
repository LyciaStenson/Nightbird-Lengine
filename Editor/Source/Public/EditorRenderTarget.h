#pragma once

#include "Core/RenderTarget.h"

#include "UIState.h"

namespace Nightbird
{
	class Engine;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapChain;
	class ModelManager;
	class ImGuiDescriptorPool;
	class UIState;
	class EditorUI;
	class ProjectManagerUI;
	
	class EditorRenderTarget : public RenderTarget
	{
	public:
		EditorRenderTarget(Renderer* renderer, VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager, Engine* engine);
		virtual ~EditorRenderTarget();
		
		virtual void Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;

		void StartProjectManager();
		void StartEditor();

	private:
		UIState* m_UIState = nullptr;

		std::unique_ptr<EditorUI> m_EditorUI;
		std::unique_ptr<ProjectManagerUI> m_ProjectManagerUI;

		std::unique_ptr<ImGuiDescriptorPool> m_DescriptorPool;
	};
}