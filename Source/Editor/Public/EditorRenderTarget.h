#pragma once

#include "Core/RenderTarget.h"

#include "UIState.h"

namespace Nightbird
{
	class Engine;
	class Renderer;
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
		EditorRenderTarget(Renderer* renderer, VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, SceneManager* sceneManager, ModelManager* modelManager, Engine* engine);
		virtual ~EditorRenderTarget() override;
		
		virtual void Render(SceneManager* sceneManager, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;

		void StartProjectManager();
		void StartEditor();

	protected:
		UIState* m_UIState = nullptr;

		GLFWwindow* m_Window = nullptr;

		std::unique_ptr<EditorUI> m_EditorUI;
		std::unique_ptr<ProjectManagerUI> m_ProjectManagerUI;

		std::unique_ptr<ImGuiDescriptorPool> m_DescriptorPool;

		Renderer* m_Renderer;
	};
}