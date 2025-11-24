#include "SceneOutliner.h"

#include <array>
#include <string>
#include <iostream>

#include "EditorUI.h"
#include "Core/Scene.h"
#include "Core/PrefabInstance.h"

namespace Nightbird
{
	SceneOutliner::SceneOutliner(Scene* scene, EditorUI* editorUI, bool open)
		: ImGuiWindow("Scene Outliner", open, ImGuiWindowProperties{true}), m_Scene(scene), m_EditorUI(editorUI)
	{

	}

	void SceneOutliner::OnRender()
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("New Scene Object"))
			{
				if (ImGui::MenuItem("New Scene Object"))
				{
					m_EditorUI->OpenWindow("Create Object Window");
				}
				if (ImGui::MenuItem("Instantiate Model"))
				{
					m_EditorUI->OpenWindow("Instantiate Model Window");
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Load Model"))
			{
				m_EditorUI->OpenWindow("Load Model Window");
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		bool dropHandled = false;
		
		for (const auto& child : m_Scene->GetRootObject()->GetChildren())
		{
			DrawSceneNode(child.get(), dropHandled);
		}

		ImVec2 space = ImGui::GetContentRegionAvail();
		ImGui::Dummy(ImVec2(space.x, std::max(24.0f, space.y)));

		if (!dropHandled && ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				SceneObject* received = *static_cast<SceneObject**>(payload->Data);
				if (received)
					received->SetParent(m_Scene->GetRootObject());
			}
			ImGui::EndDragDropTarget();
		}
	}

	void SceneOutliner::DrawSceneNode(SceneObject* object, bool& dropHandled)
	{
		if (!object)
			return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (object == m_EditorUI->GetSelectedObject())
			flags |= ImGuiTreeNodeFlags_Selected;

		if (object->GetChildren().empty() || dynamic_cast<PrefabInstance*>(object))
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());

		if (ImGui::IsItemClicked())
			m_EditorUI->SelectObject(object);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SCENE_OBJECT", &object, sizeof(SceneObject*));
			ImGui::Text(object->GetName().c_str());
			ImGui::EndDragDropSource();
		}
		
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				SceneObject* received = *static_cast<SceneObject**>(payload->Data);
				if (received && received != object)
					received->SetParent(object);
			}
			ImGui::EndDragDropTarget();
		}

		if (opened && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			for (auto& child : object->GetChildren())
			{
				DrawSceneNode(child.get(), dropHandled);
			}
			ImGui::TreePop();
		}
	}
}