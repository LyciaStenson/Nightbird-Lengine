#pragma once

#include <memory>

#include "Vulkan/RenderPass.h"
#include "Vulkan/Pipeline.h"

#include <volk.h>

namespace Nightbird
{
	class SceneManager;

	class RenderTarget
	{
	public:
		virtual ~RenderTarget() = default;
		
		virtual void Render(SceneManager* sceneManager, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) = 0;
	};
}