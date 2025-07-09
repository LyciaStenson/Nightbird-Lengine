#pragma once

#include <vector>

#include <ImGuiWindow.h>

#include <Core/SceneObjectRegistry.h>

namespace Nightbird
{
	class Scene;
	class VulkanImGuiOverlay;
	
	class CreateObjectWindow : public ImGuiWindow
	{
	public:
		CreateObjectWindow(Scene* scene, bool open = false);

		void SetObjectTypes(const std::vector<const SceneObjectDescriptor*>& objectTypes);

	protected:
		void OnRender() override;

		Scene* m_Scene;

		std::vector<const SceneObjectDescriptor*> m_ObjectTypes;
	};
}