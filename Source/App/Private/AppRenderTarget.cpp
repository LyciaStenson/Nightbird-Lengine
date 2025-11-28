#include <AppRenderTarget.h>

#include <Core/Renderer.h>
#include <Core/SceneManager.h>

#include <volk.h>

namespace Nightbird
{
	AppRenderTarget::AppRenderTarget(Renderer* renderer)
		: m_Renderer(renderer)
	{

	}

	AppRenderTarget::~AppRenderTarget()
	{

	}

	void AppRenderTarget::Render(SceneManager* sceneManager, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		renderPass->Begin(commandBuffer, framebuffer, extent);
		Camera* mainCamera = sceneManager->GetMainCamera();
		if (mainCamera)
			m_Renderer->DrawScene(sceneManager, mainCamera, commandBuffer, extent);
		renderPass->End(commandBuffer);
	}
}