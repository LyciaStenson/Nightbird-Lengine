#include "ProjectManagerUI.h"

#include <iostream>
#include <filesystem>

#include "Core/Engine.h"
#include "EditorRenderTarget.h"
#include "Vulkan/RenderPass.h"

#include "imgui_stdlib.h"

namespace Nightbird
{
	void ProjectManagerUI::Init(Engine* engine, EditorRenderTarget* renderTarget)
	{
		m_Engine = engine;
		m_RenderTarget = renderTarget;
	}
	
	void ProjectManagerUI::Render(Renderer* renderer, VulkanRenderPass* renderPass, VkCommandBuffer commandBuffer, VkFramebuffer framebuffer, VkExtent2D extent)
	{
		renderPass->Begin(commandBuffer, framebuffer, extent);
		NewFrame();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);

		ImGui::Begin("##Background", nullptr,
			ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoDocking);

		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImGui::GetIO().DisplaySize);

		ImGui::Text("Project Manager");
		
		ImGui::InputText("Project Path", &m_ProjectPath, ImGuiInputFlags_None);
		ImGui::InputText("Project Name", &m_ProjectName, ImGuiInputFlags_None);

		if (ImGui::Button("Create Project"))
		{
			CreateProject(m_ProjectPath, m_ProjectName);
		}
		
		if (ImGui::Button("Load Project"))
		{
			m_Engine->LoadProject(m_ProjectPath);
			m_RenderTarget->StartEditor();
		}

		ImGui::End();

		ImGui::PopStyleVar(3);

		Draw(commandBuffer);
		renderPass->End(commandBuffer);
	}
	
	void ProjectManagerUI::NewFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ProjectManagerUI::Draw(VkCommandBuffer commandBuffer)
	{
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	}

	void ProjectManagerUI::CreateProject(const std::string& path, const std::string& name)
	{
		std::filesystem::path source = "../../Project";
		std::filesystem::path destination = std::filesystem::path(path) / name;
		
		if (!std::filesystem::exists(source))
		{
			std::cerr << "Source project template not found!" << std::endl;
			return;
		}

		if (!std::filesystem::exists(destination))
		{
			if (!std::filesystem::create_directories(destination))
			{
				std::cerr << "Failed to create project folder!" << std::endl;
				return;
			}
		}

		for (const auto& entry : std::filesystem::recursive_directory_iterator(source))
		{
			const auto& relativePath = std::filesystem::relative(entry.path(), source);
			std::filesystem::path destinationPath = destination / relativePath;

			if (std::filesystem::is_directory(entry.path()))
			{
				std::filesystem::create_directories(destinationPath);
			}
			else if (std::filesystem::is_regular_file(entry.path()))
			{
				std::filesystem::copy_file(entry.path(), destinationPath, std::filesystem::copy_options::overwrite_existing);
			}
		}
		
		std::cout << "Project " << name << "created successfully at " << destination << std::endl;
	}
}