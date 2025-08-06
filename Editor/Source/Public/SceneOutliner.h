#pragma once

#include <vector>
#include <memory>

#include <ImGuiWindow.h>
#include <Core/SceneObject.h>

namespace Nightbird
{
	class EditorUI;
	class Scene;
	
	class SceneOutliner : public ImGuiWindow
	{
	public:
		SceneOutliner(Scene* scene, EditorUI* editorUI, bool open = true);

	protected:
		void OnRender() override;
		
		void SceneOutliner::DrawSceneNode(SceneObject* object, bool& dropHandled);

		Scene* m_Scene = nullptr;
		EditorUI* m_EditorUI = nullptr;
	};
}