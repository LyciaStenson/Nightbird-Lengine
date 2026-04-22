#include "EditorApplication.h"

#include "Core/Scene.h"
#include "Core/BackendFactory.h"
#include "Core/SceneObject.h"
#include "Core/Log.h"

#include "ProjectConfig.h"
#include "EditorBackendFactory.h"
#include "SettingsManager.h"

#include "Scene/TextSceneReader.h"

#include "Windows/BuildWindow.h"
#include "Windows/SceneWindow.h"
#include "Windows/SceneOutliner.h"
#include "Windows/Inspector.h"
#include "Windows/AssetBrowser.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include <cstdlib>

#ifdef _WIN32
#include <Windows.h>
#endif

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
		Nightbird::TypeRegistry::InitReflection();

		m_Platform = Core::CreatePlatform();
		m_Renderer = Core::CreateRenderer();

		m_ImportManager = std::make_unique<ImportManager>(m_ProjectPath.parent_path() / "Assets");
		m_AssetManager = std::make_unique<Core::AssetManager>(*m_ImportManager);

		m_Engine = std::make_unique<Core::Engine>(*m_Platform, *m_Renderer, *m_AssetManager);
	}
	
	int EditorApplication::LoadProject()
	{
		if (m_ProjectPath.empty())
		{
			Core::Log::Error("No project specified");
			return 1;
		}
		
		std::filesystem::path projectDir = m_ProjectPath.parent_path();

		const char* envPath = std::getenv("NIGHTBIRD_PATH");
		if (!envPath)
			return 1;
		
		std::filesystem::path installPath = std::filesystem::path(envPath);

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

		std::string libraryStr;
#ifdef _WIN32
		libraryStr = projectConfig.name + ".dll";
#else
		libraryStr = "lib" + projectConfig.name + ".so";
#endif

		std::filesystem::path sharedLibPath = projectDir / "Binaries" / platformStr / configStr / libraryStr;

#ifdef _WIN32
		m_ProjectLibHandle = LoadLibraryA(sharedLibPath.string().c_str());
		if (!m_ProjectLibHandle)
		{
			DWORD error = GetLastError();
			Core::Log::Error("Failed to load project: " + sharedLibPath.string());
			Core::Log::Error(std::to_string(error));
			return 1;
		}
#else
		m_ProjectLibHandle = dlopen(sharedLibPath.string().c_str(), RTLD_NOW | RTLD_GLOBAL);
		if (!m_ProjectLibHandle)
		{
			Core::Log::Error("Failed to load project: " + sharedLibPath.string());
			Core::Log::Error(dlerror());
			return 1;
		}
#endif
		Core::Log::Info("Loaded project: " + sharedLibPath.string());

#ifdef _WIN32
		auto initProjectFn = reinterpret_cast<ProjectInitFn>(GetProcAddress(m_ProjectLibHandle, "NB_InitProject"));
#else
		auto initProjectFn = reinterpret_cast<ProjectInitFn>(dlsym(m_ProjectLibHandle, "NB_InitProject"));
#endif

		if (!initProjectFn)
		{
			Core::Log::Error("Failed to find NB_InitProject in Project shared library");
			UnloadProject();
			return 1;
		}

		initProjectFn([](TypeInfo* type)
			{
				TypeRegistry::Register(type);
			}
		);

		return 0;
	}

	void EditorApplication::UnloadProject()
	{
		if (m_ProjectLibHandle)
		{
#ifdef _WIN32
			FreeLibrary(m_ProjectLibHandle);
#else
			dlclose(m_ProjectLibHandle);
#endif
			m_ProjectLibHandle = nullptr;
			m_ProjectLoaded = false;
		}
	}

	void EditorApplication::InitializeEditor()
	{
		m_EditorUIBackend = CreateEditorUIBackend(m_Engine->GetPlatform(), m_Engine->GetRenderer());
		m_EditorUIBackend->Initialize();

		InitializeImportManager();
		InitializeCookManager();

		m_EditorContext = std::make_unique<EditorContext>(*m_Engine, *m_EditorUIBackend, *m_ImportManager, *m_CookManager);

		InitializeSettings();
		InitializeWindows();
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

		m_WindowManager->AddWindow<BuildWindow>(*m_EditorContext);
		m_WindowManager->AddWindow<SceneWindow>(*m_EditorContext);
		m_WindowManager->AddWindow<SceneOutliner>(*m_EditorContext);
		m_WindowManager->AddWindow<Inspector>(*m_EditorContext);
		m_WindowManager->AddWindow<AssetBrowser>(*m_EditorContext);
		m_WindowManager->AddWindow<Editor::EditorSettingsWindow>(m_EditorSettings);
		if (m_ProjectLoaded)
			m_WindowManager->AddWindow<Editor::ProjectSettingsWindow>(m_ProjectSettings);
		m_WindowManager->AddWindow<Editor::AboutWindow>();
	}

	void EditorApplication::InitializeEditorUI()
	{
		m_EditorUI = std::make_unique<EditorUI>(*m_EditorContext, *m_WindowManager);
		m_EditorUI->ApplyTheme(EditorTheme::Dark);
	}

	void EditorApplication::InitializeImportManager()
	{
		m_ImportManager->Scan();
	}

	void EditorApplication::InitializeCookManager()
	{
		m_CookManager = std::make_unique<CookManager>("Cooked", *m_ImportManager);
	}

	void EditorApplication::RunEditorLoop()
	{
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
