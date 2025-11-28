#pragma once

#include <filesystem>

#include "ImGuiWindow.h"

namespace Nightbird
{
	class SceneManager;
	class EditorUI;

	class AssetBrowser : public ImGuiWindow
	{
	public:
		AssetBrowser(SceneManager* sceneManager, EditorUI* editorUI, bool open = true);

	protected:
		void OnRender() override;

		SceneManager* m_Scene = nullptr;

		EditorUI* m_EditorUI;
		
		std::filesystem::path m_CurrentPath;
		std::filesystem::path m_SelectedPath;
	};
}