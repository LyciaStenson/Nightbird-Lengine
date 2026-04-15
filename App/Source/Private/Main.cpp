#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/BackendFactory.h"
#include "Core/AssetLoader.h"
#include "Core/ProjectLoader.h"
#include "Core/BinarySceneReader.h"
#include "Core/TextureLoader.h"
#include "Core/MaterialLoader.h"
#include "Core/MeshLoader.h"
#include "Core/AudioLoader.h"
#include "Core/Log.h"

#include <filesystem>
#include <memory>

using namespace Nightbird::Core;

int main()
{
	Nightbird::TypeRegistry::InitReflection();

	auto platform = CreatePlatform();
	auto renderer = CreateRenderer();
	Engine engine(std::move(platform), std::move(renderer));

	auto cookedPath = engine.GetPlatform().GetCookedAssetsPath();
	AssetLoader assetLoader(cookedPath);

	ProjectInfo project = assetLoader.LoadProject();

	SceneReadResult result = assetLoader.LoadScene(project.mainSceneUUID);
	if (result.root)
	{
		auto scene = std::make_unique<Scene>();
		scene->SetActiveCamera(result.activeCamera);

		for (auto& child : result.root->GetChildren())
			scene->GetRoot()->AddChild(std::move(child));

		engine.SetScene(std::move(scene));
	}
	else
	{
		Log::Error("Failed to load main scene");
		//return -1;
	}

	while (!engine.ShouldClose())
	{
		engine.Update();
		auto& surface = engine.GetRenderer().GetDefaultSurface();
		if (!engine.GetRenderer().BeginFrame(surface))
			continue;
		if (engine.GetScene().GetActiveCamera())
			engine.GetRenderer().SubmitScene(engine.GetScene(), *engine.GetScene().GetActiveCamera());
		engine.GetRenderer().DrawScene(surface);
		engine.GetRenderer().EndFrame(surface);
	}

	return 0;
}
