#pragma once

#include <filesystem>

#include "ImGuiWindow.h"

namespace Nightbird
{
	class Scene;
	class EditorUI;

	class AssetBrowser : public ImGuiWindow
	{
	public:
		AssetBrowser(Scene* scene, EditorUI* editorUI, bool open = true);

	protected:
		void OnRender() override;

		Scene* m_Scene = nullptr;

		EditorUI* m_EditorUI;
		
		std::filesystem::path m_CurrentPath;
		std::filesystem::path m_SelectedPath;
	};
}