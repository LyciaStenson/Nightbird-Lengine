#include "Windows/Inspector.h"

#include "EditorContext.h"

#include "Core/SceneObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

NB_REFLECT_NO_FIELDS(Nightbird::Editor::Inspector, NB_PARENT(Nightbird::Editor::ImGuiWindow), NB_NO_FACTORY)

namespace Nightbird::Editor
{
	Inspector::Inspector(EditorContext& context, bool open)
		: ImGuiWindow("Inspector", open), m_Context(context)
	{

	}

	void Inspector::OnRender()
	{
		Core::SceneObject* selectedObject = m_Context.GetSelectedObject();
		if (selectedObject)
		{
			DrawFields(&selectedObject->s_TypeInfo);
		}
	}

	void Inspector::DrawFields(TypeInfo* type)
	{
		if (type->HasFields())
		{
			for (FieldInfo* field = type->Begin(); field != type->End(); ++field)
			{
				ImGui::PushID(field->name);
				
				if (field->kind == FieldKind::Int32)
				{
					int32_t value = 0;
					ImGui::DragInt(field->name, &value);
				}
				else if (field->kind == FieldKind::UInt32)
				{
					int32_t signedValue = 0;
					ImGui::DragInt(field->name, &signedValue);
					uint32_t value = signedValue;
				}
				else if (field->kind == FieldKind::Float)
				{
					float value = 0.0f;
					ImGui::DragFloat(field->name, &value);
				}
				else if (field->kind == FieldKind::Bool)
				{
					bool value = false;
					ImGui::Checkbox(field->name, &value);
				}
				else if (field->kind == FieldKind::String)
				{
					std::string value = "";
					char buffer[256];
					std::strncpy(buffer, value.c_str(), sizeof(buffer));
					ImGui::InputText(field->name, buffer, sizeof(buffer));
				}
				else if (field->kind == FieldKind::Vector2)
				{
					glm::vec3 value = glm::vec3();
					ImGui::DragFloat3(field->name, glm::value_ptr(value), 0.01f);
				}
				else if (field->kind == FieldKind::Quat)
				{
					glm::quat value = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
					ImGui::DragFloat4(field->name, glm::value_ptr(value), 0.01f);
				}
				else if (field->kind == FieldKind::Object)
				{
					if (field->type)
						DrawFields(field->type);
				}
				else if (field->kind == FieldKind::Unknown)
				{
					ImGui::LabelText("Unknown field: %s", field->name);
				}

				ImGui::PopID();
			}
		}
	}
}
