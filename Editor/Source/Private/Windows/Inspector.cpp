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
		Core::SceneObject* selected = m_Context.GetSelectedObject();
		if (!selected)
		{
			ImGui::TextDisabled("Select an object to edit");
			return;
		}

		const TypeInfo* type = selected->GetTypeInfo();
		ImGui::Text("%s", type->name);
		ImGui::Separator();

		DrawFields(selected, type);
	}

	void Inspector::DrawFields(void* object, const TypeInfo* type)
	{
		if (!object || !type)
			return;

		if (type->parent)
			DrawFields(object, type->parent);

		if (!type->HasFields())
			return;

		for (const FieldInfo* field = type->Begin(); field != type->End(); ++field)
		{
			ImGui::PushID(field->name);

			switch (field->kind)
			{
				case FieldKind::Bool:
				{
					bool* value = field->GetPtrAs<bool>(object);
					ImGui::Checkbox(field->name, value);
					break;
				}
				case FieldKind::Int32:
				{
					int32_t* value = field->GetPtrAs<int32_t>(object);
					ImGui::DragInt(field->name, value);
					break;
				}
				case FieldKind::UInt32:
				{
					uint32_t* value = field->GetPtrAs<uint32_t>(object);
					ImGui::DragScalar(field->name, ImGuiDataType_U32, value);
					break;
				}
				case FieldKind::Float:
				{
					float* value = field->GetPtrAs<float>(object);
					ImGui::DragFloat(field->name, value, 0.01f);
					break;
				}
				case FieldKind::String:
				{
					std::string* value = field->GetPtrAs<std::string>(object);
					char buffer[256];
					std::strncpy(buffer, value->c_str(), sizeof(buffer));
					if (ImGui::InputText(field->name, buffer, sizeof(buffer)))
						*value = buffer;
					break;
				}
				case FieldKind::Vector2:
				{
					glm::vec2* value = field->GetPtrAs<glm::vec2>(object);
					ImGui::DragFloat2(field->name, glm::value_ptr(*value), 0.01f);
					break;
				}
				case FieldKind::Vector3:
				{
					glm::vec3* value = field->GetPtrAs<glm::vec3>(object);
					ImGui::DragFloat3(field->name, glm::value_ptr(*value), 0.01f);
					break;
				}
				case FieldKind::Vector4:
				{
					glm::vec4* value = field->GetPtrAs<glm::vec4>(object);
					ImGui::DragFloat4(field->name, glm::value_ptr(*value), 0.01f);
					break;
				}
				case FieldKind::Quat:
				{
					glm::quat* value = field->GetPtrAs<glm::quat>(object);
					ImGui::DragFloat4(field->name, glm::value_ptr(*value), 0.01f);
					break;
				}
				case FieldKind::Object:
				{
					if (field->type)
					{
						if (ImGui::TreeNodeEx(field->name, ImGuiTreeNodeFlags_DefaultOpen))
						{
							void* fieldObject = field->GetPtr(object);
							DrawFields(fieldObject, field->type);
							ImGui::TreePop();
						}
					}
					break;
				}
				case FieldKind::UUID:
				{
					ImGui::TextDisabled("UUID: %s", field->name);
					break;
				}
				case FieldKind::Unknown:
				{
					ImGui::TextDisabled("Unknown field: %s", field->name);
					break;
				}
				default:
				{
					ImGui::TextDisabled("Missing field type: %s", field->name);
					break;
				}
			}

			ImGui::PopID();
		}
	}
}
