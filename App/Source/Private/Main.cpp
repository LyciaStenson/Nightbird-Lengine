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

using namespace Nightbird::Core;

int main()
{
	auto platform = CreatePlatform();
	auto renderer = CreateRenderer();

	Engine engine(std::move(platform), std::move(renderer));
	
	std::vector<Vertex> vertices =
	{
		{ glm::vec3( 0.0f,  0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.5f, 0.0f), glm::vec2(0), glm::vec2(0) },
		{ glm::vec3( 0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0), glm::vec2(0) },
		{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f), glm::vec2(0), glm::vec2(0) }
	};

	std::vector<uint16_t> indices = { 0, 1, 2 };

	auto material = std::make_shared<Material>();
	material->baseColorFactor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	std::vector<MeshPrimitive> primitives = { MeshPrimitive(vertices, indices, material) };
	auto mesh = std::make_shared<Mesh>(primitives);

	auto meshInstance = std::make_unique<MeshInstance>("MeshInstance", mesh);
	
	auto camera = std::make_unique<Camera>("Camera");
	camera->transform.position = glm::vec3(0.0f, 0.0f, 5.0f);

	Camera* cameraPtr = camera.get();

	engine.GetScene().GetRoot()->AddChild(std::move(meshInstance));
	engine.GetScene().GetRoot()->AddChild(std::move(camera));
	engine.GetScene().SetActiveCamera(cameraPtr);

	engine.Run();

	return 0;
}