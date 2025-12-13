#pragma once

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

#include "Windows/ImGuiWindow.h"

#include <volk.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

namespace Nightbird
{
	class Renderer;
	class VulkanRenderPass;

	class UIState
	{
	public:
		virtual ~UIState() = default;
		
		virtual void Render(Renderer* renderer, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent) = 0;

	private:
		virtual void NewFrame() = 0;
		virtual void Draw(VkCommandBuffer commandBuffer) = 0;
	};
}