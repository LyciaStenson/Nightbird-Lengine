#include <EditorRenderTarget.h>

#include <Core/Engine.h>
#include <Core/Renderer.h>
#include <Core/Scene.h>
#include <Vulkan/Texture.h>
#include <EditorUI.h>
#include <SceneWindow.h>

#include <Vulkan/Instance.h>
#include <Vulkan/Device.h>
#include <Vulkan/SwapChain.h>
#include <Core/ModelManager.h>

namespace Nightbird
{
	EditorRenderTarget::EditorRenderTarget(Renderer* renderer, VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager)
		: RenderTarget(renderer)
	{
		editorUI = std::make_unique<EditorUI>(instance, device, swapChain, renderPass, glfwWindow, scene, modelManager);
	}

	EditorRenderTarget::~EditorRenderTarget()
	{

	}
	
	void EditorRenderTarget::Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		SceneWindow* sceneWindow = static_cast<SceneWindow*>(editorUI->GetWindow("Scene Window"));
		if (sceneWindow)
		{
			if (sceneWindow->ShouldResize())
				sceneWindow->RecreateRenderResources();
			sceneWindow->GetColorTexture()->TransitionToColor(commandBuffer);
		}

		if (sceneWindow)
		{
			sceneWindow->BeginRenderPass(commandBuffer);
			Camera* mainCamera = scene->GetMainCamera();
			if (mainCamera)
				renderer->DrawScene(scene, mainCamera, commandBuffer, sceneWindow->GetExtent());
			sceneWindow->EndRenderPass(commandBuffer);
		}

		if (sceneWindow)
			sceneWindow->GetColorTexture()->TransitionToShaderRead(commandBuffer);

		renderPass->Begin(commandBuffer, framebuffer, extent);
		editorUI->Render(commandBuffer);
		renderPass->End(commandBuffer);
	}
}