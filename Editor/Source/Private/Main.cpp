#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/BackendFactory.h"
#include "Core/Log.h"

#include "EditorUIRenderer.h"
#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "EditorContext.h"
#include "EditorBackendFactory.h"
#include "EditorUI.h"

#include "SettingsManager.h"

#include "WindowManager.h"
#include "Windows/SceneOutliner.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include "EditorTheme.h"

#include <rttr/library.h>

#include <string>

int main(int argc, char** argv)
{
	auto platform = Nightbird::Core::CreatePlatform();
	auto renderer = Nightbird::Core::CreateRenderer();
	Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));

	bool projectLoaded = false;
	std::string projectPath;
	if (argc > 1)
	{
		rttr::library project(argv[1]);
		projectLoaded = project.load();
		if (projectLoaded)
		{
			projectPath = std::string(argv[1]);
			Nightbird::Core::Log::Info("Loaded project: " + projectPath);
		}
		else
		{
			Nightbird::Core::Log::Error("Failed to load project: " + std::string(argv[1]));
			Nightbird::Core::Log::Error(project.get_error_string().to_string());
		}
	}
	else
	{
		Nightbird::Core::Log::Info("No project specified");
	}

	Nightbird::Editor::EditorContext context(engine);

	Nightbird::Editor::SettingsManager settingsManager;
	Nightbird::Editor::EditorSettings editorSettings = settingsManager.LoadEditorSettings();
	Nightbird::Editor::ProjectSettings projectSettings;
	if (projectLoaded)
		projectSettings = settingsManager.LoadProjectSettings(projectPath);

	Nightbird::Editor::WindowManager windowManager;
	windowManager.AddWindow<Nightbird::Editor::SceneOutliner>(context);
	windowManager.AddWindow<Nightbird::Editor::EditorSettingsWindow>(editorSettings);
	if (projectLoaded)
		windowManager.AddWindow<Nightbird::Editor::ProjectSettingsWindow>(projectSettings);
	windowManager.AddWindow<Nightbird::Editor::AboutWindow>();

	auto editorUIRenderer = Nightbird::Editor::CreateEditorUIRenderer(engine.GetPlatform(), engine.GetRenderer());
	editorUIRenderer->Initialize();

	Nightbird::Editor::EditorUI editorUI(windowManager);
	editorUI.ApplyTheme(Nightbird::Editor::EditorTheme::Dark);

	while (!engine.ShouldClose())
	{
		engine.Update();
		auto& surface = engine.GetRenderer().GetDefaultSurface();
		engine.GetRenderer().BeginFrame(surface);
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
