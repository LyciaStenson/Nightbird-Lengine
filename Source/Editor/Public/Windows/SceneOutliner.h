#pragma once

#include <vector>
#include <memory>

#include "ImGuiWindow.h"
#include "Core/SceneObject.h"

namespace Nightbird
{
	class EditorUI;
	class SceneManager;
	
	class SceneOutliner : public ImGuiWindow
	{
	public:
		SceneOutliner(SceneManager* sceneManager, EditorUI* editorUI, bool open = true);

	protected:
		void OnRender() override;
		
		void DrawSceneNode(SceneObject* object, bool& dropHandled);

		SceneManager* m_Scene = nullptr;
		EditorUI* m_EditorUI = nullptr;
	};
}