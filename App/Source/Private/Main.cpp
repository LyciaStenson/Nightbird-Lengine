#include "Core/Engine.h"
#include "Core/BackendFactory.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Input/Provider.h"

#include "Core/Vertex.h"
#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/MeshInstance.h"
#include "Core/Camera.h"
#include "Core/Log.h"

#include "Load/AssetLoader.h"
#include "Load/MeshLoader.h"
#include "Load/MaterialLoader.h"
#include "Load/TextureLoader.h"
#include "Scene/BinarySceneReader.h"

#include <uuid.h>

using namespace Nightbird;

int main()
{
	auto platform = Core::CreatePlatform();
	auto renderer = Core::CreateRenderer();

	Core::Engine engine(std::move(platform), std::move(renderer));
	
	//std::vector<Core::Vertex> vertices =
	//{
	//	{ glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 0.0f), glm::vec2(0), glm::vec2(0) },
	//	{ glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0), glm::vec2(0) },
	//	{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0), glm::vec2(0) }
	//};

	//std::vector<uint16_t> indices = { 0, 1, 2 };

	//auto material = std::make_shared<Core::Material>();
	//material->baseColorFactor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//std::vector<Core::MeshPrimitive> primitives = { Core::MeshPrimitive(vertices, indices, material) };
	//auto mesh = std::make_shared<Core::Mesh>(primitives);

	//auto meshInstance = std::make_unique<Core::MeshInstance>("MeshInstance", mesh);
	//
	//auto camera = std::make_unique<Core::Camera>("Camera");
	//camera->transform.position = glm::vec3(0.0f, 0.0f, 5.0f);

	//Core::Camera* cameraPtr = camera.get();

	//engine.GetScene().GetRoot()->AddChild(std::move(meshInstance));
	//engine.GetScene().GetRoot()->AddChild(std::move(camera));
	//engine.GetScene().SetActiveCamera(cameraPtr);

	Load::AssetLoader assetLoader("Cooked/Desktop");

	auto sceneUUID = uuids::uuid::from_string("df7d18af-fc6b-4b96-bf57-198dbad27486");
	if (!sceneUUID)
	{
		Core::Log::Error("Invalid scene UUID");
		return -1;
	}

	auto scene = assetLoader.LoadScene(*sceneUUID);
	if (!scene)
	{
		Core::Log::Error("Failed to load scene");
		return -1;
	}

	engine.SetScene(std::move(scene));
	
	engine.Run();

	return 0;
}