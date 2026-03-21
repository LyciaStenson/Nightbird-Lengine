#include "Core/Engine.h"
#include "Core/BackendFactory.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"

#include "Input/InputProvider.h"
#include "Audio/AudioProvider.h"
#include "Core/AudioAsset.h"

#include "Core/Vertex.h"
#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/MeshInstance.h"
#include "Core/Camera.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

#include "Load/AssetLoader.h"
#include "Load/MeshLoader.h"
#include "Load/MaterialLoader.h"
#include "Load/TextureLoader.h"
#include "Load/AudioLoader.h"
#include "Scene/BinarySceneReader.h"

#include <uuid.h>

#include <filesystem>

using namespace Nightbird;

int main()
{
	auto platform = Core::CreatePlatform();
	auto cookedPath = platform->GetCookedAssetsPath();
	auto renderer = Core::CreateRenderer();

	Core::Engine engine(std::move(platform), std::move(renderer));

	engine.Initialize();

	auto sceneUUID = uuids::uuid::from_string("664cd96f-2a3d-4eef-b0bd-3fc0b0efdab7");
	if (!sceneUUID)
	{
		Core::Log::Error("Invalid scene UUID");
		return -1;
	}

	Load::AssetLoader assetLoader(cookedPath);

	auto scene = assetLoader.LoadScene(*sceneUUID);
	if (!scene)
	{
		Core::Log::Error("Failed to load scene");
		return -1;
	}

	engine.SetScene(std::move(scene));

	engine.RunLoop();
	engine.Shutdown();

	return 0;
}
