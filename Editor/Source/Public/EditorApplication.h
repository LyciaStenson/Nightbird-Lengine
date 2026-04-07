#pragma once

#include "Core/Engine.h"

#include "SettingsManager.h"
#include "EditorSettings.h"
#include "ProjectSettings.h"

#include "EditorUIBackend.h"
#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "EditorUI.h"
#include "EditorContext.h"
#include "WindowManager.h"
#include "Import/ImportManager.h"

#include <memory>
#include <filesystem>

namespace Nightbird::Editor
{
	class EditorApplication
	{
	public:
		EditorApplication() = default;
		~EditorApplication() = default;

		int Run(int argc, char** argv);

	private:
		void InitializeEngine();
		int LoadProject();
		void InitializeEditor();
		void InitializeSettings();
		void InitializeWindows();
		void InitializeEditorUI();
		void InitializeImportManager();
		void RunEditorLoop();
		void Update();
		void Render();
		void Shutdown();

		std::unique_ptr<Core::Engine> m_Engine;
		std::unique_ptr<EditorUIBackend> m_EditorUIBackend;
		std::unique_ptr<EditorUI> m_EditorUI;
		std::unique_ptr<EditorContext> m_EditorContext;
		std::unique_ptr<WindowManager> m_WindowManager;
		std::unique_ptr<ImportManager> m_ImportManager;
		
		SettingsManager m_SettingsManager;
		EditorSettings m_EditorSettings;
		ProjectSettings m_ProjectSettings;

		std::filesystem::path m_ProjectPath;
		bool m_ProjectLoaded = false;
	};
}