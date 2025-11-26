#include "EditorRenderTarget.h"

#include "Core/Engine.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"
#include "Vulkan/Texture.h"
#include "Windows/SceneWindow.h"
#include "EditorCamera.h"

#include "UIState.h"
#include "EditorUI.h"
#include "ProjectManagerUI.h"

#include "ImGuiDescriptorPool.h"

#include "Vulkan/Instance.h"
#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"
#include "Core/ModelManager.h"

namespace Nightbird
{
	EditorRenderTarget::EditorRenderTarget(Renderer* renderer, VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager, Engine* engine)
		: RenderTarget(renderer)
	{
		m_DescriptorPool = std::make_unique<ImGuiDescriptorPool>(device);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& imGuiIO = ImGui::GetIO(); (void)imGuiIO;
		imGuiIO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		imGuiIO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		imGuiIO.Fonts->AddFontFromFileTTF("Assets/Fonts/RobotoFlex-Regular.ttf", 16.0f);

		ImGuiStyle& style = ImGui::GetStyle();
		style.TabRounding = 4.0f;
		style.FrameRounding = 4.0f;
		style.WindowRounding = 6.0f;
		style.WindowPadding = ImVec2(8.0f, 8.0f);
		style.FramePadding = ImVec2(8.0f, 6.0f);
		style.ItemSpacing = ImVec2(10.0f, 8.0f);
		style.TabBarBorderSize = 0.0f;
		style.WindowBorderSize = 0.0f;
		style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

		ImGui_ImplGlfw_InitForVulkan(glfwWindow, true);

		ImGui_ImplVulkan_InitInfo imGuiInitInfo = {};
		imGuiInitInfo.ApiVersion = VK_API_VERSION_1_3;
		imGuiInitInfo.Instance = instance->Get();
		imGuiInitInfo.PhysicalDevice = device->GetPhysical();
		imGuiInitInfo.Device = device->GetLogical();
		imGuiInitInfo.QueueFamily = device->graphicsQueueFamily;
		imGuiInitInfo.Queue = device->graphicsQueue;
		imGuiInitInfo.DescriptorPool = m_DescriptorPool->Get();
		imGuiInitInfo.MinImageCount = swapChain->GetMinImageCount();
		imGuiInitInfo.ImageCount = swapChain->GetImageCount();
		imGuiInitInfo.PipelineInfoMain.RenderPass = renderPass->Get();
		imGuiInitInfo.PipelineInfoMain.Subpass = 0;
		imGuiInitInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&imGuiInitInfo);
		
		m_ProjectManagerUI = std::make_unique<ProjectManagerUI>();

		m_EditorUI = std::make_unique<EditorUI>();
		m_EditorUI->Init(instance, device, swapChain, renderPass, modelManager, engine, glfwWindow, scene);
	}

	EditorRenderTarget::~EditorRenderTarget()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}
	
	void EditorRenderTarget::Render(Scene* scene, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		if (m_UIState)
		{
			m_UIState->Render(renderer, renderPass, commandBuffer, framebuffer, extent);
		}
	}

	void EditorRenderTarget::StartProjectManager()
	{
		m_UIState = m_ProjectManagerUI.get();
	}

	void EditorRenderTarget::StartEditor()
	{
		m_UIState = m_EditorUI.get();
	}
}