#include "Windows/Inspector.h"

#include "EditorContext.h"

#include "Core/SceneObject.h"
#include "Core/Transform.h"

namespace Nightbird::Editor
{
	NB_REFLECT_NO_FIELDS(Inspector, NB_PARENT(ImGuiWindow), NB_NO_FACTORY)

	Inspector::Inspector(EditorContext& context, bool open)
		: ImGuiWindow("Inspector", open), m_Context(context)
	{

	}

	void Inspector::OnRender()
	{
		Core::SceneObject* selectedObject = m_Context.GetSelectedObject();
		if (selectedObject)
		{
			//DrawProperties(rttr::instance(*selectedObject));
		}
	}

	//void Inspector::DrawProperties(rttr::instance instance)
	//{
	//	rttr::type type = instance.get_derived_type();

	//	for (auto& property : type.get_properties())
	//	{
	//		if (!property.is_valid() || property.is_readonly())
	//			continue;

	//		rttr::variant variant = property.get_value(instance);
	//		rttr::type propertyType = variant.get_type();
	//		std::string label = property.get_name().to_string();

	//		ImGui::PushID(label.c_str());

	//		if (propertyType == rttr::type::get<int>())
	//		{
	//			int value = variant.get_value<int>();
	//			if (ImGui::DragInt(label.c_str(), &value))
	//				property.set_value(instance, value);
	//		}
	//		else if (propertyType == rttr::type::get<float>())
	//		{
	//			float value = variant.get_value<float>();
	//			if (ImGui::DragFloat(label.c_str(), &value))
	//				property.set_value(instance, value);
	//		}
	//		else if (propertyType == rttr::type::get<bool>())
	//		{
	//			bool value = variant.get_value<bool>();
	//			if (ImGui::Checkbox(label.c_str(), &value))
	//				property.set_value(instance, value);
	//		}
	//		else if (propertyType == rttr::type::get<std::string>())
	//		{
	//			std::string value = variant.get_value<std::string>();
	//			char buffer[256];
	//			std::strncpy(buffer, value.c_str(), sizeof(buffer));
	//			if (ImGui::InputText(label.c_str(), buffer, sizeof(buffer)))
	//				property.set_value(instance, std::string(buffer));
	//		}
	//		else if (propertyType == rttr::type::get<Core::Vector3>())
	//		{
	//			Core::Vector3 value = variant.get_value<Core::Vector3>();
	//			if (ImGui::DragFloat3(label.c_str(), &value.x, 0.01f))
	//				property.set_value(instance, value);
	//		}
	//		else if (propertyType == rttr::type::get<Core::Transform>())
	//		{
	//			Core::Transform transform = variant.get_value<Core::Transform>();

	//			ImGui::Text("%s", label.c_str());
	//			ImGui::PushID((label + "_Transform").c_str());

	//			if (ImGui::DragFloat3("Position", &transform.position.x, 0.01f))
	//				property.set_value(instance, transform);
	//			if (ImGui::DragFloat4("Rotation", &transform.rotation.x))
	//				property.set_value(instance, transform);
	//			if (ImGui::DragFloat3("Scale", &transform.scale.x, 0.01f))
	//				property.set_value(instance, transform);

	//			ImGui::PopID();
	//		}

	//		ImGui::PopID();
	//	}
	//}
}
