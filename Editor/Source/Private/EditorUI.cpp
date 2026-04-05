#include "EditorUI.h"

#include "WindowManager.h"

#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

namespace Nightbird::Editor
{
	EditorUI::EditorUI(WindowManager& windowManager)
		: m_WindowManager(windowManager)
	{

	}

	void EditorUI::ApplyTheme(EditorTheme theme)
	{
		ImGuiStyle& style = ImGui::GetStyle();

		switch (theme)
		{
			case EditorTheme::Light:
				ImGui::StyleColorsLight();
				break;
			case EditorTheme::Dark:
				ImGui::StyleColorsDark();
				break;
		}

		style.FramePadding = ImVec2(8.0f, 8.0f);
		style.WindowPadding = ImVec2(12.0f, 12.0f);
	}

	void EditorUI::Render()
	{
		MainMenuBar();

		ImGui::DockSpaceOverViewport();

		m_WindowManager.Render();
	}

	void EditorUI::MainMenuBar()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Exit"))
				{

				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Editor Settings"))
				{
					if (auto* window = m_WindowManager.GetWindow<EditorSettingsWindow>())
						window->SetOpen(true);
				}

				if (ImGui::MenuItem("Project Settings"))
				{
					if (auto* window = m_WindowManager.GetWindow<ProjectSettingsWindow>())
						window->SetOpen(true);
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Window"))
			{
				for (auto& window : m_WindowManager.GetWindows())
				{
					bool open = window->IsOpen();
					if (ImGui::MenuItem(window->GetTitle().c_str(), nullptr, &open))
						window->SetOpen(open);
				}
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("About"))
				{
					if (auto* aboutWindow = m_WindowManager.GetWindow<AboutWindow>())
						aboutWindow->SetOpen(true);
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}
