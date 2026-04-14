#include "Windows/SceneOutliner.h"

#include "EditorContext.h"

#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	NB_OBJECT_NO_FACTORY_IMPL(Nightbird::Editor::SceneOutliner, Nightbird::Editor::ImGuiWindow)

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

		DrawAddObjectPopup();

		ImGui::Separator();

		bool dropHandled = false;

		for (const auto& child : m_Context.GetEngine().GetScene().GetRoot()->GetChildren())
			DrawSceneNode(child.get(), dropHandled);

		ImVec2 space = ImGui::GetContentRegionAvail();
		ImGui::Dummy(ImVec2(space.x, std::max(24.0f, space.y)));

		if (!dropHandled && ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				Core::SceneObject* received = *static_cast<Core::SceneObject**>(payload->Data);
				if (received)
					received->SetParent(m_Context.GetEngine().GetScene().GetRoot());
			}
			ImGui::EndDragDropTarget();
		}
	}

	void SceneOutliner::DrawAddObjectPopup()
	{
		static std::vector<std::string> objectTypeNames;
		static std::vector<rttr::type> objectTypes;
		static bool objectTypesDirty = true;

		if (objectTypesDirty)
		{
			objectTypeNames.clear();
			objectTypes.clear();

			for (auto& type : rttr::type::get_types())
			{
				if (type.is_derived_from(rttr::type::get<Core::SceneObject>()))
				{
					auto constructor = type.get_constructor(); // Missing name { rttr::type::get<std::string>() }
					if (constructor.is_valid())
					{
						objectTypeNames.push_back(type.get_name().to_string());
						objectTypes.push_back(type);
					}
				}
			}

			objectTypesDirty = false;
		}

		if (ImGui::Button("Add"))
		{
			ImGui::OpenPopup("AddObject");
		}

		if (ImGui::BeginPopup("AddObject"))
		{
			for (size_t i = 0; i < objectTypeNames.size() && i < objectTypes.size(); ++i)
			{
				if (ImGui::MenuItem(objectTypeNames[i].c_str()))
				{
					rttr::variant variant = objectTypes[i].create();
					if (variant.is_valid())
					{
						Core::SceneObject* rawObject = variant.get_value<Core::SceneObject*>();
						rawObject->SetName(objectTypeNames[i]);
						std::unique_ptr<Core::SceneObject> object(rawObject);

						if (auto* selectObject = m_Context.GetSelectedObject())
							selectObject->AddChild(std::move(object));
						else
							m_Context.GetEngine().GetScene().GetRoot()->AddChild(std::move(object));
					}
				}
			}
			ImGui::EndPopup();
		}
	}

	void SceneOutliner::DrawSceneNode(Core::SceneObject* object, bool& dropHandled)
	{
		if (!object)
			return;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (object == m_Context.GetSelectedObject())
			flags |= ImGuiTreeNodeFlags_Selected;

		if (object->GetChildren().empty() || object->HasSourceScene())
			flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

		bool opened = ImGui::TreeNodeEx(object, flags, "%s", object->GetName().c_str());

		if (ImGui::IsItemClicked())
			m_Context.SelectObject(object);

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("SCENE_OBJECT", &object, sizeof(Core::SceneObject*));
			ImGui::Text(object->GetName().c_str());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_OBJECT"))
			{
				Core::SceneObject* received = *static_cast<Core::SceneObject**>(payload->Data);
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
