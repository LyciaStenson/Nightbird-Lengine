#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/BackendFactory.h"
#include "Core/Log.h"

#include "ProjectConfig.h"

#include "EditorUIRenderer.h"
#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "EditorContext.h"
#include "EditorBackendFactory.h"
#include "EditorUI.h"

#include "SettingsManager.h"

#include "WindowManager.h"
#include "Windows/SceneOutliner.h"
#include "Windows/Inspector.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include "EditorTheme.h"

#include <rttr/library.h>

#include <string>
#include <cstdlib>
#include <filesystem>

static std::filesystem::path GetInstallPath()
{
	const char* envPath = std::getenv("NIGHTBIRD_PATH");
	if (!envPath)
	{
		Nightbird::Core::Log::Error("Failed to find NIGHTBIRD_PATH environment variable");
		return "";
	}
	return std::filesystem::path(envPath);
}

int main(int argc, char** argv)
{
	std::filesystem::path projectPath;
	bool projectLoaded = false;
	if (argc > 1)
	{
		projectPath = argv[1];
		std::filesystem::path projectDir = projectPath.parent_path();
		std::filesystem::path installPath = GetInstallPath();

		Nightbird::Editor::ProjectConfig projectConfig = Nightbird::Editor::LoadProjectConfig(projectPath);
		if (projectConfig.name.empty())
		{
			Nightbird::Core::Log::Error("Invalid project name in: " + projectPath.string());
			return 1;
		}
		
		std::filesystem::path premakePath = projectDir / "premake5.lua";
		if (!std::filesystem::exists(premakePath))
		{
			Nightbird::Editor::GeneratePremake(projectConfig, installPath, projectDir);
			Nightbird::Core::Log::Info("Generated premake5.lua for " + projectConfig.name + ", now build the project");
			return 0;
		}

		// EDITORDEBUG ONLY
		std::string configStr = "EditorDebug";

		std::string platformStr;

#ifdef _WIN32
		platformStr = "windows-x86_64";
#else
		platformStr = "linux-x86_64";
#endif

		std::filesystem::path sharedLibPath = projectDir / "Binaries" / platformStr / "EditorDebug" / projectConfig.name;
		rttr::library projectLib(sharedLibPath.string());
		projectLoaded = projectLib.load();
		if (projectLoaded)
		{
			Nightbird::Core::Log::Info("Loaded project: " + sharedLibPath.string());
		}
		else
		{
			Nightbird::Core::Log::Error("Failed to load project: " + sharedLibPath.string());
			Nightbird::Core::Log::Info("Make sure to build the project");
			Nightbird::Core::Log::Error(projectLib.get_error_string().to_string());
		}
	}
	else
	{
		Nightbird::Core::Log::Info("No project specified");
	}

	auto platform = Nightbird::Core::CreatePlatform();
	auto renderer = Nightbird::Core::CreateRenderer();
	Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));

	Nightbird::Editor::EditorContext context(engine);

	Nightbird::Editor::SettingsManager settingsManager;
	Nightbird::Editor::EditorSettings editorSettings = settingsManager.LoadEditorSettings();
	Nightbird::Editor::ProjectSettings projectSettings;
	if (projectLoaded)
		projectSettings = settingsManager.LoadProjectSettings(projectPath.string());

	Nightbird::Editor::WindowManager windowManager;
	windowManager.AddWindow<Nightbird::Editor::SceneOutliner>(context);
	windowManager.AddWindow<Nightbird::Editor::Inspector>(context);
	windowManager.AddWindow<Nightbird::Editor::EditorSettingsWindow>(editorSettings);
	if (projectLoaded)
		windowManager.AddWindow<Nightbird::Editor::ProjectSettingsWindow>(projectSettings);
	windowManager.AddWindow<Nightbird::Editor::AboutWindow>();

	auto editorUIRenderer = Nightbird::Editor::CreateEditorUIRenderer(engine.GetPlatform(), engine.GetRenderer());
	editorUIRenderer->Initialize();

	Nightbird::Editor::EditorUI editorUI(windowManager);
	editorUI.ApplyTheme(Nightbird::Editor::EditorTheme::Dark);

	settingsManager.SaveEditorSettings(editorSettings);
	settingsManager.SaveProjectSettings(projectSettings, "");

	while (!engine.ShouldClose())
	{
		engine.Update();
		auto& surface = engine.GetRenderer().GetDefaultSurface();
		if (!engine.GetRenderer().BeginFrame(surface))
			continue;
		if (engine.GetScene().GetActiveCamera())
			engine.GetRenderer().SubmitScene(engine.GetScene(), *engine.GetScene().GetActiveCamera());
		engine.GetRenderer().DrawScene(surface);
		
		editorUIRenderer->BeginFrame();

		editorUI.Render();

		editorUIRenderer->EndFrame();

		engine.GetRenderer().EndFrame(surface);
	}

	editorUIRenderer->Shutdown();

	return 0;
}
