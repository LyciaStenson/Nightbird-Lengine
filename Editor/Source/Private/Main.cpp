#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/BackendFactory.h"
#include "Core/TypeInfo.h"
#include "Core/Log.h"

#include "ProjectConfig.h"

#include "EditorUIBackend.h"
#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "EditorContext.h"
#include "EditorBackendFactory.h"
#include "EditorUI.h"

#include "SettingsManager.h"

#include "WindowManager.h"
#include "Windows/SceneWindow.h"
#include "Windows/SceneOutliner.h"
#include "Windows/Inspector.h"
#include "Windows/EditorSettingsWindow.h"
#include "Windows/ProjectSettingsWindow.h"
#include "Windows/AboutWindow.h"

#include "Import/ImportManager.h"
#include "Scene/TextSceneReader.h"
#include "Scene/TextSceneWriter.h"

#include "EditorTheme.h"

#include <rttr/library.h>

#include <string>
#include <cstdlib>
#include <filesystem>

using namespace Nightbird;

static std::filesystem::path GetInstallPath()
{
	const char* envPath = std::getenv("NIGHTBIRD_PATH");
	if (!envPath)
	{
		Core::Log::Error("Failed to find NIGHTBIRD_PATH environment variable");
		return "";
	}
	return std::filesystem::path(envPath);
}

uuids::uuid GenerateUUID()
{
	std::random_device randomDevice;

	auto seedData = std::array<int, std::mt19937::state_size>{};
	std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
	std::seed_seq seq(std::begin(seedData), std::end(seedData));
	std::mt19937 generator(seq);
	uuids::uuid_random_generator gen{generator};

	return gen();
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

		Editor::ProjectConfig projectConfig = Editor::LoadProjectConfig(projectPath);
		if (projectConfig.name.empty())
		{
			Core::Log::Error("Invalid project name in: " + projectPath.string());
			return 1;
		}
		
		std::filesystem::path premakePath = projectDir / "premake5.lua";
		if (!std::filesystem::exists(premakePath))
		{
			Editor::GeneratePremake(projectConfig, installPath, projectDir);
			Core::Log::Info("Generated premake5.lua for " + projectConfig.name + ", now build the project");
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

		std::filesystem::path sharedLibPath = projectDir / "Binaries" / platformStr / configStr / projectConfig.name;
		rttr::library projectLib(sharedLibPath.string());
		projectLoaded = projectLib.load();
		if (projectLoaded)
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
	else
	{
		Core::Log::Info("No project specified");
	}

	auto platform = Core::CreatePlatform();
	auto renderer = Core::CreateRenderer();
	Core::Engine engine(std::move(platform), std::move(renderer));

	auto editorUIBackend = Editor::CreateEditorUIBackend(engine.GetPlatform(), engine.GetRenderer());
	editorUIBackend->Initialize();

	Editor::EditorContext context(engine, *editorUIBackend);

	Editor::SettingsManager settingsManager;
	Editor::EditorSettings editorSettings = settingsManager.LoadEditorSettings();
	Editor::ProjectSettings projectSettings;
	if (projectLoaded)
		projectSettings = settingsManager.LoadProjectSettings(projectPath.string());
	
	Editor::WindowManager windowManager;
	Editor::SceneWindow& sceneWindow = windowManager.AddWindow<Editor::SceneWindow>(context);
	windowManager.AddWindow<Editor::SceneOutliner>(context);
	windowManager.AddWindow<Editor::Inspector>(context);
	windowManager.AddWindow<Editor::EditorSettingsWindow>(editorSettings);
	if (projectLoaded)
		windowManager.AddWindow<Editor::ProjectSettingsWindow>(projectSettings);
	windowManager.AddWindow<Editor::AboutWindow>();

	Editor::EditorUI editorUI(windowManager);
	editorUI.ApplyTheme(Editor::EditorTheme::Dark);

	//settingsManager.SaveEditorSettings(editorSettings);
	//settingsManager.SaveProjectSettings(projectSettings, "");

	Editor::ImportManager importManager("Assets");
	importManager.Scan();
	
	//auto sceneUUID = uuids::uuid::from_string("357fba9b-33ab-4aeb-8fa1-82dd9cd20765");
	//if (sceneUUID.has_value())
	//{
	//	auto result = importManager.LoadScene(*sceneUUID);
	//	if (auto* cadence = Cast<Core::SpatialObject>(result.get()))
	//	{
	//		cadence->m_Transform.position = glm::vec3(0.0f, 0.0f, 5.0f);
	//		cadence->m_Transform.scale = glm::vec3(1.0f);

	//		engine.GetScene().GetRoot()->AddChild(std::move(result));
	//	}
	//	else
	//	{
	//		Core::Log::Error("Failed to import model");
	//	}
	//}
	//else
	//{
	//	Core::Log::Error("Failed to load scene UUID");
	//}

	//auto camera = std::make_unique<Core::Camera>("Camera");
	//camera->m_Transform.position = glm::vec3(0.0f, 1.5f, 5.0f);
	//Core::Camera* cameraPtr = camera.get();
	
	//engine.GetScene().GetRoot()->AddChild(std::move(camera));
	//engine.GetScene().SetActiveCamera(cameraPtr);

	//Editor::TextSceneWriter sceneWriter;
	//sceneWriter.Write(engine.GetScene(), "ClubPenguin", GenerateUUID(), "Assets/Scenes/ClubPenguin.ntscene");

	Editor::TextSceneReader sceneReader(importManager);
	engine.SetScene(sceneReader.Read("Assets/Scenes/ClubPenguin.ntscene").scene);

	while (!engine.ShouldClose())
	{
		engine.Update();

		auto& surface = engine.GetRenderer().GetDefaultSurface();
		if (!engine.GetRenderer().BeginFrame(surface))
			continue;

		if (sceneWindow.IsOpen())
		{
			if (sceneWindow.NeedsResize())
				sceneWindow.Resize(sceneWindow.GetPendingWidth(), sceneWindow.GetPendingHeight());
			
			engine.GetRenderer().SubmitScene(engine.GetScene(), sceneWindow.GetCamera());

			engine.GetRenderer().BeginFrame(sceneWindow.GetSurface());
			engine.GetRenderer().DrawScene(sceneWindow.GetSurface());
			engine.GetRenderer().EndFrame(sceneWindow.GetSurface());
		}
		
		editorUIBackend->BeginFrame();
		editorUI.Render();
		editorUIBackend->EndFrame();

		engine.GetRenderer().EndFrame(surface);
	}

	editorUIBackend->Shutdown();

	return 0;
}
