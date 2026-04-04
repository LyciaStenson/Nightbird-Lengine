#include "EditorUI.h"

#include "WindowManager.h"

namespace Nightbird::Editor
{
	EditorUI::EditorUI(WindowManager& windowManager)
		: m_WindowManager(windowManager)
	{

	}

	void EditorUI::Render()
	{
		RenderMainMenuBar();

		m_WindowManager.Render();
	}

	void EditorUI::RenderMainMenuBar()
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
		}
		ImGui::EndMainMenuBar();
	}
}