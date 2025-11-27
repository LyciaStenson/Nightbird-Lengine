#include "ProjectManagerUI.h"

#include "Core/Engine.h"
#include "Vulkan/RenderPass.h"

#include "imgui_stdlib.h"

namespace Nightbird
{
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
		
		ImGui::InputText("Enter Project Path", &projectPath, ImGuiInputFlags_None);
		
		if (ImGui::Button("Load Project"))
		{
			//m_Engine->LoadProject(projectPath);
			m_ShouldClose = true;
		}

		ImGui::End();

		ImGui::PopStyleVar(3);

		Draw(commandBuffer);
		renderPass->End(commandBuffer);
	}

	bool ProjectManagerUI::ShouldClose()
	{
		return m_ShouldClose;
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
}