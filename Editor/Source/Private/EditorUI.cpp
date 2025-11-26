#include "EditorUI.h"

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Vulkan/Instance.h"
#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/RenderPass.h"

#include "Core/MeshInstance.h"
#include "Core/Camera.h"
#include "Core/Scene.h"
#include "Core/ModelManager.h"
#include "Core/Engine.h"
#include "Core/Renderer.h"

#include "Windows/SceneOutliner.h"
#include "Windows/LoadModelWindow.h"
#include "Windows/CreateObjectWindow.h"
#include "Windows/InstantiateModelWindow.h"
#include "Windows/Inspector.h"
#include "Windows/AssetBrowser.h"
#include "Windows/SceneWindow.h"
#include "Windows/AboutWindow.h"

#include "EditorCamera.h"

namespace Nightbird
{
	void EditorUI::Init(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, ModelManager* modelManager, Engine* engine, GLFWwindow* glfwWindow, Scene* scene)
	{
		m_Window = glfwWindow;

		m_Windows["Scene Outliner"] = std::make_unique<SceneOutliner>(scene, this);
		m_Windows["Load Model Window"] = std::make_unique<LoadModelWindow>(modelManager);
		m_Windows["Instantiate Model Window"] = std::make_unique<InstantiateModelWindow>(modelManager, scene);
		m_Windows["Create Object Window"] = std::make_unique<CreateObjectWindow>(scene);
		m_Windows["Inspector"] = std::make_unique<Inspector>(scene, this);
		m_Windows["Asset Browser"] = std::make_unique<AssetBrowser>(scene, this);
		m_Windows["Scene Window"] = std::make_unique<SceneWindow>(engine, this, device, swapChain->GetColorFormat(), swapChain->GetDepthFormat());
		m_Windows["About"] = std::make_unique<AboutWindow>();
	}

	ImGuiWindow* EditorUI::GetWindow(const std::string& title)
	{
		if (m_Windows.count(title))
		{
			return m_Windows[title].get();
		}
		return nullptr;
	}
	
	SceneObject* EditorUI::GetSelectedObject() const
	{
		return m_SelectedObject;
	}

	void EditorUI::SelectObject(SceneObject* object)
	{
		m_SelectedObject = object;
	}
	
	void EditorUI::Render(Renderer* renderer, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		SceneWindow* sceneWindow = static_cast<SceneWindow*>(GetWindow("Scene Window"));
		if (sceneWindow)
		{
			if (sceneWindow->ShouldResize())
				sceneWindow->RecreateRenderResources();

			sceneWindow->GetColorTexture()->TransitionToColor(commandBuffer);
			
			sceneWindow->BeginRenderPass(commandBuffer);
			renderer->DrawScene(m_Scene, sceneWindow->GetEditorCamera(), commandBuffer, sceneWindow->GetExtent());
			sceneWindow->EndRenderPass(commandBuffer);

			sceneWindow->GetColorTexture()->TransitionToShaderRead(commandBuffer);
		}

		renderPass->Begin(commandBuffer, framebuffer, extent);
		NewFrame();

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Scene (Binary)"))
				{
					std::string path = "Assets/Scenes/Scene.scene";
					m_Scene->SaveSceneBIN(path);
				}
				if (ImGui::MenuItem("Save Scene (Text)"))
				{
					std::string path = "Assets/Scenes/Scene.tscene";
					m_Scene->SaveSceneJSON(path);
				}
				if (ImGui::MenuItem("Quit"))
					glfwSetWindowShouldClose(m_Window, true);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Scene Window"))
				{
					if (m_Windows.count("Scene Window"))
						m_Windows["Scene Window"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Scene Outliner"))
				{
					if (m_Windows.count("Scene Outliner"))
						m_Windows["Scene Outliner"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Inspector"))
				{
					if (m_Windows.count("Inspector"))
						m_Windows["Inspector"]->SetOpen(true);
				}
				if (ImGui::MenuItem("Asset Browser"))
				{
					if (m_Windows.count("Asset Browser"))
						m_Windows["Asset Browser"]->SetOpen(true);
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					if (m_Windows.count("About"))
						m_Windows["About"]->SetOpen(true);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		for (auto& [_, window] : m_Windows)
		{
			window->Render();
		}
		
		Draw(commandBuffer);
		renderPass->End(commandBuffer);
	}

	bool EditorUI::ShouldClose()
	{
		return false;
	}

	void EditorUI::OpenWindow(const std::string& title)
	{
		if (m_Windows.count(title))
			m_Windows[title]->SetOpen(true);
	}
	
	void EditorUI::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		
		ImGui::DockSpaceOverViewport();
	}
	
	void EditorUI::Draw(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}
}