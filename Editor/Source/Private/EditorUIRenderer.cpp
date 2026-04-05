#include "EditorUIRenderer.h"

#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"

#include <imgui.h>

namespace Nightbird::Editor
{
	EditorUIRenderer::EditorUIRenderer(std::unique_ptr<ImGuiPlatform> platform, std::unique_ptr<ImGuiRenderer> renderer)
		: m_Platform(std::move(platform)), m_Renderer(std::move(renderer))
	{

	}

	void EditorUIRenderer::Initialize()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		m_Platform->Initialize();
		m_Renderer->Initialize();

		ImGui::StyleColorsDark();
	}

	void EditorUIRenderer::Shutdown()
	{
		m_Renderer->Shutdown();
		m_Platform->Shutdown();

		ImGui::DestroyContext();
	}

	void EditorUIRenderer::BeginFrame()
	{
		m_Platform->NewFrame();
		m_Renderer->NewFrame();
		ImGui::NewFrame();
	}

	void EditorUIRenderer::EndFrame()
	{
		ImGui::Render();
		m_Renderer->RenderDrawData();
	}
}
