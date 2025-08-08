#pragma once

#include <memory>

#include <ImGuiWindow.h>

#include <volk.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanRenderPass;
	class VulkanTexture;
	class VulkanRenderPass;
	class Scene;
	class Engine;
	class EditorCamera;
	
	class SceneWindow : public ImGuiWindow
	{
	public:
		SceneWindow(Engine* engine, VulkanDevice* device, VkFormat colorFormat, VkFormat depthFormat, bool open = true);
		~SceneWindow() override;

		EditorCamera* GetEditorCamera() const;

		VulkanTexture* GetColorTexture() const;

		VkFramebuffer GetFramebuffer() const;
		VkExtent2D GetExtent() const;

		ImVec2 GetViewportPos() const;
		ImVec2 GetViewportSize() const;
		
		void BeginRenderPass(VkCommandBuffer commandBuffer);
		void EndRenderPass(VkCommandBuffer commandBuffer);

		void RecreateRenderResources();

		bool ShouldResize();

	protected:
		void OnRender() override;
		
		void CreateRenderResources();
		void CleanupRenderResources();

		static ImGuiWindowProperties BuildProperties();

		std::unique_ptr<EditorCamera> editorCamera;

		Engine* engine = nullptr;
		
		VulkanDevice* device = nullptr;

		VulkanTexture* colorTexture = nullptr;
		VulkanTexture* depthTexture = nullptr;

		std::unique_ptr<VulkanRenderPass> renderPass;

		VkFramebuffer framebuffer = VK_NULL_HANDLE;

		VkExtent2D extent = {800, 600};
		VkFormat colorFormat;
		VkFormat depthFormat;

		ImTextureID imGuiTextureId = 0;

		ImVec2 viewportPos = ImVec2(0.0f, 0.0f);
		ImVec2 viewportSize = ImVec2(800, 600);

		unsigned int currentWidth = 800;
		unsigned int currentHeight = 600;

		bool shouldResize = false;
	};
}