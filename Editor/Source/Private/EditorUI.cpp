#include <EditorUI.h>

#include <vector>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <Vulkan/Instance.h>
#include <Vulkan/Device.h>
#include <Vulkan/SwapChain.h>
#include <Vulkan/RenderPass.h>

#include <Core/MeshInstance.h>
#include <Core/Camera.h>
#include <Core/Scene.h>
#include <Core/ModelManager.h>
#include <Core/Engine.h>

#include <ImGuiDescriptorPool.h>
#include <SceneOutliner.h>
#include <LoadModelWindow.h>
#include <CreateObjectWindow.h>
#include <InstantiateModelWindow.h>
#include <Inspector.h>
#include <AssetBrowser.h>
#include <SceneWindow.h>
#include <AboutWindow.h>
#include <EditorCamera.h>

namespace Nightbird
{
	EditorUI::EditorUI(VulkanInstance* instance, VulkanDevice* device, VulkanSwapChain* swapChain, VulkanRenderPass* renderPass, GLFWwindow* glfwWindow, Scene* scene, ModelManager* modelManager, Engine* engine)
		: m_Window(glfwWindow), m_Scene(scene)
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
		imGuiInitInfo.RenderPass = renderPass->Get();
		imGuiInitInfo.Subpass = 0;
		imGuiInitInfo.MinImageCount = swapChain->GetMinImageCount();
		imGuiInitInfo.ImageCount = swapChain->GetImageCount();
		imGuiInitInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&imGuiInitInfo);
		
		m_Windows["Scene Outliner"] = std::make_unique<SceneOutliner>(scene, this);
		m_Windows["Load Model Window"] = std::make_unique<LoadModelWindow>(modelManager);
		m_Windows["Instantiate Model Window"] = std::make_unique<InstantiateModelWindow>(modelManager, scene);
		m_Windows["Create Object Window"] = std::make_unique<CreateObjectWindow>(scene);
		m_Windows["Inspector"] = std::make_unique<Inspector>(scene, this);
		m_Windows["Asset Browser"] = std::make_unique<AssetBrowser>(scene, this);
		m_Windows["Scene Window"] = std::make_unique<SceneWindow>(engine, device, swapChain->GetColorFormat(), swapChain->GetDepthFormat());
		m_Windows["About"] = std::make_unique<AboutWindow>();
	}
	
	EditorUI::~EditorUI()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
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
	
	void EditorUI::Render(VkCommandBuffer commandBuffer)
	{
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
		
		auto* sceneWindow = dynamic_cast<SceneWindow*>(GetWindow("Scene Window"));
		if (sceneWindow && m_SelectedObject)
		{
			ImGuizmo::SetOrthographic(false);

			ImVec2 viewportPos = sceneWindow->GetViewportPos();
			ImVec2 viewportSize = sceneWindow->GetViewportSize();
			
			ImGuizmo::SetRect(viewportPos.x, viewportPos.y, viewportSize.x, viewportSize.y);

			EditorCamera* editorCamera = sceneWindow->GetEditorCamera();
			if (!editorCamera)
				return;

			glm::mat4 view = editorCamera->GetViewMatrix();
			glm::mat4 proj = editorCamera->GetProjectionMatrix(viewportSize.x, viewportSize.y);

			glm::mat4 model = m_SelectedObject->GetLocalMatrix();

			ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), m_CurrentGizmoOperation, m_CurrentGizmoMode, glm::value_ptr(model));

			std::cout << "Hovered: " << ImGuizmo::IsOver() << ", Using: " << ImGuizmo::IsUsing() << ", MouseDown: " << ImGui::IsMouseDown(ImGuiMouseButton_Left) << std::endl;

			if (ImGuizmo::IsUsing())
			{
				std::cout << "Using" << std::endl;
				
				glm::vec3 translation = glm::vec3(model[3]);
				
				glm::vec3 scale = glm::vec3(1.0f);
				scale.x = glm::length(glm::vec3(model[0]));
				scale.y = glm::length(glm::vec3(model[1]));
				scale.z = glm::length(glm::vec3(model[2]));

				glm::mat3 rotationMat;
				rotationMat[0] = glm::vec3(model[0]) / scale.x;
				rotationMat[1] = glm::vec3(model[1]) / scale.y;
				rotationMat[2] = glm::vec3(model[2]) / scale.z;

				glm::quat rotation = glm::quat_cast(rotationMat);

				m_SelectedObject->transform.position = translation;
				m_SelectedObject->transform.rotation = rotation;
				m_SelectedObject->transform.scale = scale;
			}
		}

		Draw(commandBuffer);
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
		
		ImGui::DockSpaceOverViewport();
	}
	
	void EditorUI::Draw(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}
}