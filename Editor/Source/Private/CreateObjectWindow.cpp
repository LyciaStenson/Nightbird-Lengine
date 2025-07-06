#include <CreateObjectWindow.h>

#include <array>

#include <Core/Scene.h>
#include <Core/Transform.h>
#include <Core/SceneObject.h>

using namespace Nightbird;

CreateObjectWindow::CreateObjectWindow(Scene* scene, bool open)
	: ImGuiWindow("Create Object", open, ImGuiWindowProperties{false, true, ImVec2(400, 600)}), m_Scene(scene)
{

}

void CreateObjectWindow::SetObjectTypes(const std::vector<const CustomObjectDescriptor*>& objectTypes)
{
	m_ObjectTypes = objectTypes;
}

void CreateObjectWindow::OnRender()
{
	//const std::array<const std::string, 4> objectTypes = {"Empty Scene Object", "Mesh Instance", "Point Light", "Camera"};
	static std::vector<std::string> objectTypes;
	static bool objectTypesDirty = true;
	
	if (objectTypesDirty)
	{
		objectTypes.clear();

		objectTypes.push_back("Empty Scene Object");
		objectTypes.push_back("Mesh Instance");
		objectTypes.push_back("Point Light");
		objectTypes.push_back("Camera");

		for (const auto& objectType : m_ObjectTypes)
		{
			objectTypes.push_back(objectType->name);
		}

		objectTypesDirty = false;
	}

	static int selectedObjectType = -1;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

	ImGui::Text("Create an empty scene object.");

	ImGui::PopStyleVar(); // Pop spacing

	ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.0f, 0.5f));
	for (int i = 0; i < objectTypes.size(); ++i)
	{
		if (ImGui::Selectable(objectTypes[i].c_str(), selectedObjectType == i, 0, ImVec2(0.0f, 30.0f)))
			selectedObjectType = i;
	}
	ImGui::PopStyleVar(); // Pop selectable text align

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 15.0f));

	ImGui::Dummy(ImVec2(0.0f, 0.0f));

	ImGui::BeginDisabled(selectedObjectType == -1);
	if (ImGui::Button("Create"))
	{
		Transform transform;

		if (selectedObjectType < 4)
		{
			switch (selectedObjectType)
			{
			case 0:
				m_Scene->CreateSceneObject("Empty Scene Object", transform.position, transform.rotation, transform.scale, nullptr);
				break;
			case 1:
				break;
			case 2:
				m_Scene->CreatePointLight("Point Light", transform.position, transform.rotation, transform.scale, nullptr);
				break;
			case 3:
				Camera * camera = m_Scene->CreateCamera("Camera", transform.position, transform.rotation, transform.scale, nullptr);
				break;
			}
		}
		else
		{
			int customIndex = selectedObjectType - 4;
			if (customIndex >= 0 && customIndex < static_cast<int>(m_ObjectTypes.size()))
			{
				auto* objectDesc = m_ObjectTypes[customIndex];
				SceneObject* rawObject = objectDesc->create("Custom Object");
				if (rawObject)
				{
					std::unique_ptr<SceneObject, SceneObjectDeleter> customObject(rawObject);
					m_Scene->AddSceneObject(std::move(customObject));
				}
			}
		}

		m_Open = false;
		selectedObjectType = -1;
	}
	ImGui::EndDisabled();

	ImGui::PopStyleVar();
}