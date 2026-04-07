#include "EditorApplication.h"

#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/Scene.h"
#include "Core/BackendFactory.h"
#include "Core/Log.h"

#include "ProjectConfig.h"
#include "EditorBackendFactory.h"
#include "SettingsManager.h"

#include "Scene/TextSceneReader.h"

#include "Windows/SceneWindow.h"
#include "Windows/SceneOutliner.h"
#include "Windows/Inspector.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include <rttr/library.h>

#include <cstdlib>

namespace Nightbird::Editor
{
	int EditorApplication::Run(int argc, char** argv)
	{
		if (argc > 1)
			m_ProjectPath = argv[1];
		
		InitializeEngine();

		int result = LoadProject();
		if (result != 0)
			return result;

		InitializeEditor();
		RunEditorLoop();
		Shutdown();

		return 0;
	}

	void EditorApplication::InitializeEngine()
	{
		auto platform = Core::CreatePlatform();
		auto renderer = Core::CreateRenderer();

		m_Engine = std::make_unique<Core::Engine>(std::move(platform), std::move(renderer));
	}

	int EditorApplication::LoadProject()
	{
		if (m_ProjectPath.empty())
		{
			Core::Log::Error("No project specified");
			return 0;
		}
		
		std::filesystem::path projectDir = m_ProjectPath.parent_path();

		const char* envPath = std::getenv("NIGHTBIRD_PATH");
		if (!envPath)
			return 1;
		
		std::filesystem::path installPath = std::filesystem::path(std::getenv("NIGHTBIRD_PATH"));

		ProjectConfig projectConfig = LoadProjectConfig(m_ProjectPath);
		if (projectConfig.name.empty())
		{
			Core::Log::Error("Invalid project name in: " + m_ProjectPath.string());
			return 1;
		}

		std::filesystem::path premakePath = projectDir / "premake5.lua";
		if (!std::filesystem::exists(premakePath))
		{
			GeneratePremake(projectConfig, installPath, projectDir);
			Core::Log::Info("Generated premake5.lua for " + projectConfig.name + ", now build the project");
			return 1;
		}

		// EDITORDEBUG ONLY
		std::string configStr = "EditorDebug";

		std::string platformStr;
#ifdef _WIN32
		platformStr = "windows-x86_64";
#else
		platformStr = "linux-x86_64";
#endif

		std::filesystem::path sharedLibPath = projectDir / "Binaries" / platformStr / configStr / projectConfig.name;
		rttr::library projectLib(sharedLibPath.string());
		m_ProjectLoaded = projectLib.load();
		if (m_ProjectLoaded)
		{
			Core::Log::Info("Loaded project: " + sharedLibPath.string());
		}
		else
		{
			Core::Log::Error("Failed to load project: " + sharedLibPath.string());
			Core::Log::Info("Make sure to build the project");
			Core::Log::Error(projectLib.get_error_string().to_string());
		}
	}

	void EditorApplication::InitializeEditor()
	{
		m_EditorUIBackend = CreateEditorUIBackend(m_Engine->GetPlatform(), m_Engine->GetRenderer());
		m_EditorUIBackend->Initialize();

		m_EditorContext = std::make_unique<EditorContext>(*m_Engine, *m_EditorUIBackend);

		InitializeSettings();
		InitializeWindows();
		InitializeImportManager();
		InitializeEditorUI();
	}

	void EditorApplication::InitializeSettings()
	{
		m_EditorSettings = m_SettingsManager.LoadEditorSettings();
		if (m_ProjectLoaded)
			m_ProjectSettings = m_SettingsManager.LoadProjectSettings(m_ProjectPath.string());
	}

	void EditorApplication::InitializeWindows()
	{
		m_WindowManager = std::make_unique<WindowManager>();
		
		m_WindowManager->AddWindow<SceneWindow>(*m_EditorContext);
		m_WindowManager->AddWindow<SceneOutliner>(*m_EditorContext);
		m_WindowManager->AddWindow<Inspector>(*m_EditorContext);
		m_WindowManager->AddWindow<Editor::EditorSettingsWindow>(m_EditorSettings);
		if (m_ProjectLoaded)
			m_WindowManager->AddWindow<Editor::ProjectSettingsWindow>(m_ProjectSettings);
		m_WindowManager->AddWindow<Editor::AboutWindow>();
	}

	void EditorApplication::InitializeEditorUI()
	{
		m_EditorUI = std::make_unique<EditorUI>(*m_WindowManager);
		m_EditorUI->ApplyTheme(EditorTheme::Dark);
	}

	void EditorApplication::InitializeImportManager()
	{
		m_ImportManager = std::make_unique<ImportManager>("Assets");
		m_ImportManager->Scan();
	}

	void EditorApplication::RunEditorLoop()
	{
		TextSceneReader sceneReader(*m_ImportManager);
		m_Engine->SetScene(sceneReader.Read("Assets/Scenes/ClubPenguin.ntscene").scene);

		while (!m_Engine->ShouldClose())
		{
			Update();
			Render();
		}
	}

	void EditorApplication::Update()
	{
		m_Engine->Update();
	}

	void EditorApplication::Render()
	{
		auto& surface = m_Engine->GetRenderer().GetDefaultSurface();
		if (!m_Engine->GetRenderer().BeginFrame(surface))
			return;

		auto* sceneWindow = m_WindowManager->GetWindow<Editor::SceneWindow>();
		if (sceneWindow && sceneWindow->IsOpen())
		{
			if (sceneWindow->NeedsResize())
				sceneWindow->Resize(sceneWindow->GetPendingWidth(), sceneWindow->GetPendingHeight());

			m_Engine->GetRenderer().SubmitScene(m_Engine->GetScene(), sceneWindow->GetCamera());

			m_Engine->GetRenderer().BeginFrame(sceneWindow->GetSurface());
			m_Engine->GetRenderer().DrawScene(sceneWindow->GetSurface());
			m_Engine->GetRenderer().EndFrame(sceneWindow->GetSurface());
		}

		m_EditorUIBackend->BeginFrame();
		m_EditorUI->Render();
		m_EditorUIBackend->EndFrame();

		m_Engine->GetRenderer().EndFrame(surface);
	}

	void EditorApplication::Shutdown()
	{
		m_EditorUIBackend->Shutdown();
	}
}