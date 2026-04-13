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
	auto platform = CreatePlatform();
	auto renderer = CreateRenderer();
	Engine engine(std::move(platform), std::move(renderer));

	auto cookedPath = engine.GetPlatform().GetCookedAssetsPath();
	AssetLoader assetLoader(cookedPath);

	ProjectInfo project = assetLoader.LoadProject();
	auto mainScene = assetLoader.LoadScene(project.mainSceneUUID);
	if (!mainScene)
	{
		Log::Error("Failed to load main scene");
		return -1;
	}
	engine.SetScene(std::move(mainScene));

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
