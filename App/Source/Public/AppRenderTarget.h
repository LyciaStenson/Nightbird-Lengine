#pragma once

#include <Core/RenderTarget.h>

namespace Nightbird
{
	class AppRenderTarget : public RenderTarget
	{
	public:
		AppRenderTarget(Renderer* renderer);
		virtual ~AppRenderTarget();

		void Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) override;
	};
}