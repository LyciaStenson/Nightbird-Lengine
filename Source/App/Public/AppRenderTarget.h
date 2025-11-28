#pragma once

#include <Core/RenderTarget.h>

namespace Nightbird
{
	class Renderer;

	class AppRenderTarget : public RenderTarget
	{
	public:
		AppRenderTarget(Renderer* renderer);
		virtual ~AppRenderTarget() override;

		void Render(SceneManager* sceneManager, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;

	protected:
		Renderer* m_Renderer;
	};
}