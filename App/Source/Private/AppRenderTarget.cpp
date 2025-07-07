#include <AppRenderTarget.h>

#include <Core/Renderer.h>

#include <volk.h>

namespace Nightbird
{
	AppRenderTarget::AppRenderTarget(Renderer* renderer)
		: RenderTarget(renderer)
	{

	}

	AppRenderTarget::~AppRenderTarget()
	{

	}

	void AppRenderTarget::Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		renderPass->Begin(commandBuffer, framebuffer, extent);
		renderer->DrawScene(scene, commandBuffer, extent);
		renderPass->End(commandBuffer);
	}
}