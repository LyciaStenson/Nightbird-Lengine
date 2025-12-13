#pragma once

#include "UIState.h"

namespace Nightbird
{
	class VulkanInstance;
	class VulkanDevice;
	class VulkanSwapChain;
	class VulkanRenderPass;
	class Renderer;
	class SceneObject;
	class SceneManager;
	class ModelManager;
	class Engine;
	
	class EditorUI : public UIState
	{
	public:
		void Init(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, ModelManager* modelManager, Engine* engine, GLFWwindow* glfwWindow, SceneManager* sceneManager);
		
		ImGuiWindow* GetWindow(const std::string& title);
		
		SceneObject* GetSelectedObject() const;
		void SelectObject(SceneObject* object);
		
		virtual void Render(Renderer* renderer, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;
		
		void OpenWindow(const std::string& title);

	private:
		GLFWwindow* m_Window = nullptr;

		SceneManager* m_Scene = nullptr;
		SceneObject* m_SelectedObject = nullptr;

		std::unordered_map<std::string, std::unique_ptr<ImGuiWindow>> m_Windows;
		
		virtual void NewFrame() override;
		virtual void Draw(VkCommandBuffer commandBuffer) override;
	};
}