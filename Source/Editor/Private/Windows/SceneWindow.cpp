#include "Windows/SceneWindow.h"

#include <array>
#include <iostream>

#include <ImGuizmo.h>

#include "Vulkan/Device.h"
#include "Vulkan/RenderPass.h"
#include "Vulkan/Texture.h"
#include "Core/Engine.h"
#include "Core/SceneManager.h"
#include "Core/MeshInstance.h"
#include "EditorUI.h"
#include "EditorCamera.h"
#include "Input.h"

namespace Nightbird
{
	SceneWindow::SceneWindow(Engine* engine, EditorUI* editorUI, VulkanDevice* device, VkFormat colorFormat, VkFormat depthFormat, bool open)
		: ImGuiWindow("Scene", open, BuildProperties()), engine(engine), editorUI(editorUI), device(device), colorFormat(colorFormat), depthFormat(depthFormat)
	{
		renderPass = std::make_unique<VulkanRenderPass>(device, colorFormat, depthFormat, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
		
		CreateRenderResources();
		
		editorCamera = std::make_unique<EditorCamera>("EditorCamera");
	}

	SceneWindow::~SceneWindow()
	{
		CleanupRenderResources();
	}

	EditorCamera* SceneWindow::GetEditorCamera() const
	{
		return editorCamera.get();
	}

	VulkanTexture* SceneWindow::GetColorTexture() const
	{
		return colorTexture;
	}

	VkFramebuffer SceneWindow::GetFramebuffer() const
	{
		return framebuffer;
	}

	VkExtent2D SceneWindow::GetExtent() const
	{
		return extent;
	}
	
	void SceneWindow::BeginRenderPass(VkCommandBuffer commandBuffer)
	{
		renderPass->Begin(commandBuffer, framebuffer, extent);
	}

	void SceneWindow::EndRenderPass(VkCommandBuffer commandBuffer)
	{
		renderPass->End(commandBuffer);
	}

	void SceneWindow::RecreateRenderResources()
	{
		vkDeviceWaitIdle(device->GetLogical());

		CleanupRenderResources();
		CreateRenderResources();
		shouldResize = false;
	}

	bool SceneWindow::ShouldResize()
	{
		return shouldResize;
	}

	void SceneWindow::OnRender()
	{
		static bool rightMouseHeld = false;
		static bool firstFrame = true;

		if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
		{
			rightMouseHeld = true;
			firstFrame = true;
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			rightMouseHeld = false;

		if (rightMouseHeld && ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			if (firstFrame)
			{
				double mouseX, mouseY;
				Input::Get().GetCursorPos(mouseX, mouseY);
				editorCamera->SetLastMousePos(mouseX, mouseY);
				firstFrame = false;
			}
			editorCamera->Tick(engine->GetDeltaTime());
		}
		
		ImVec2 size = ImGui::GetContentRegionAvail();
		ImVec2 pos = ImGui::GetCursorScreenPos();

		unsigned int newWidth = std::max(1, (int)size.x);
		unsigned int newHeight = std::max(1, (int)size.y);

		if (newWidth != currentWidth || newHeight != currentHeight)
		{
			currentWidth = newWidth;
			currentHeight = newHeight;

			extent = {newWidth, newHeight};

			shouldResize = true;
		}

		ImGui::Image(imGuiTextureId, ImVec2((float)extent.width, (float)extent.height));

		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
		
		ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

		glm::mat4 view = editorCamera->GetViewMatrix();
		glm::mat4 proj = editorCamera->GetProjectionMatrix(size.x, size.y);

		SceneObject* selectedObject = editorUI->GetSelectedObject();
		if (!selectedObject)
			return;
		
		SpatialObject* selectedSpatialObject = dynamic_cast<SpatialObject*>(selectedObject);
		if (!selectedSpatialObject)
			return;

		glm::mat4 model = selectedSpatialObject->GetLocalMatrix();

		if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
		{
			if (ImGui::IsKeyPressed(ImGuiKey_W))
				currentGizmoOperation = ImGuizmo::TRANSLATE;

			if (ImGui::IsKeyPressed(ImGuiKey_E))
				currentGizmoOperation = ImGuizmo::ROTATE;

			if (ImGui::IsKeyPressed(ImGuiKey_R))
				currentGizmoOperation = ImGuizmo::SCALE;
		}
		
		ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(proj), currentGizmoOperation, currentGizmoMode, glm::value_ptr(model));
		
		if (ImGuizmo::IsUsing())
		{
			glm::vec3 translation, eulerDegrees, scale;

			ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), &translation[0], &eulerDegrees[0], &scale[0]);
			
			selectedSpatialObject->transform.position = translation;

			selectedSpatialObject->transform.eulerCache = eulerDegrees;
			selectedSpatialObject->transform.rotation = glm::quat(glm::radians(eulerDegrees));

			selectedSpatialObject->transform.scale = scale;
		}
	}

	void SceneWindow::CreateRenderResources()
	{
		colorTexture = new VulkanTexture(device, extent.width, extent.height, colorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		imGuiTextureId = reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(colorTexture->GetSampler(), colorTexture->GetImageView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));

		depthTexture = new VulkanTexture(device, extent.width, extent.height, depthFormat, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

		std::array<VkImageView, 2> attachments =
		{
			colorTexture->GetImageView(),
			depthTexture->GetImageView()
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass->Get();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device->GetLogical(), &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
		{
			std::cerr << "Failed to create framebuffers" << std::endl;
		}
	}

	void SceneWindow::CleanupRenderResources()
	{
		//if (imGuiTextureId != 0)
		//{
		//	ImGui_ImplVulkan_RemoveTexture(reinterpret_cast<VkDescriptorSet>(imGuiTextureId));
		//	imGuiTextureId = 0;
		//}
		if (colorTexture)
		{
			delete colorTexture;
			colorTexture = nullptr;
		}
		if (depthTexture)
		{
			delete depthTexture;
			depthTexture = nullptr;
		}
		if (framebuffer != VK_NULL_HANDLE)
		{
			vkDestroyFramebuffer(device->GetLogical(), framebuffer, nullptr);
			framebuffer = VK_NULL_HANDLE;
		}
	}

	ImGuiWindowProperties SceneWindow::BuildProperties()
	{
		ImGuiWindowProperties properties;
		properties.changePadding = true;
		properties.padding = ImVec2(2.0f, 2.0f);

		return properties;
	}
}