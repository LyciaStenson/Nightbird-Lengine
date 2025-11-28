#pragma once

#include "ImGuiWindow.h"

#include <rttr/type.h>
#include <rttr/registration.h>

namespace Nightbird
{
	class EditorUI;
	class SceneObject;
	class SceneManager;

	class Inspector : public ImGuiWindow
	{
	public:
		Inspector(SceneManager* sceneManager, EditorUI* editorUI, bool open = true);

	protected:
		void OnRender() override;
		
		void RenderProperties(rttr::instance instance);
		
		SceneManager* m_Scene = nullptr;
		EditorUI* m_EditorUI = nullptr;

		SceneObject* m_SelectedObject = nullptr;
	};
}