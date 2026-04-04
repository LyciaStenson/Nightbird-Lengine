#include "Windows/SceneOutliner.h"

#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

namespace Nightbird::Editor
{
	SceneOutliner::SceneOutliner(EditorContext& context, bool open)
		: ImGuiWindow("Scene Outliner", open, { ImGuiWindowFlags_MenuBar, ImVec2(300, 500) }), m_Context(context)
	{

	}

	void SceneOutliner::OnRender()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("View"))
			{
				ImGui::MenuItem("Expand All");
				ImGui::MenuItem("CollapseAll");
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGui::Text("Scene Outline");

		ImGui::Separator();
	}
}