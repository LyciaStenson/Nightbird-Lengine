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

#include "WindowManager.h"
#include "Windows/SceneOutliner.h"

#include "EditorUI.h"

#include <rttr/library.h>
#include <imgui.h>

#include <string>

int main(int argc, char** argv)
{
	auto runEditor = []()
	{
		auto platform = Nightbird::Core::CreatePlatform();
		auto renderer = Nightbird::Core::CreateRenderer();
		Nightbird::Core::Engine engine(std::move(platform), std::move(renderer));

		Nightbird::Editor::EditorContext context(engine);

		Nightbird::Editor::WindowManager windowManager;
		windowManager.AddWindow<Nightbird::Editor::SceneOutliner>(context);

		Nightbird::Editor::EditorUI editorUI(windowManager);

		auto editorUIRenderer = Nightbird::Editor::CreateEditorUIRenderer(engine.GetPlatform(), engine.GetRenderer());
		editorUIRenderer->Initialize();

		while (!engine.ShouldClose())
		{
			engine.Update();
			auto& surface = engine.GetRenderer().GetDefaultSurface();
			engine.GetRenderer().BeginFrame(surface);
			if (engine.GetScene().GetActiveCamera())
				engine.GetRenderer().SubmitScene(engine.GetScene(), *engine.GetScene().GetActiveCamera());
			engine.GetRenderer().DrawScene(surface);

			editorUIRenderer->BeginFrame();
			ImGui::DockSpaceOverViewport();

			editorUI.Render();

			editorUIRenderer->EndFrame();

			engine.GetRenderer().EndFrame(surface);
		}

		editorUIRenderer->Shutdown();
	};

	if (argc > 1)
	{
		rttr::library project(argv[1]);
		if (project.load())
		{
			Nightbird::Core::Log::Info("Loaded project: " + std::string(argv[1]));

			for (auto& type : rttr::type::get_types())
				Nightbird::Core::Log::Info("Registered type: " + type.get_name().to_string());

			runEditor();
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
		runEditor();
	}

	return 0;
}
