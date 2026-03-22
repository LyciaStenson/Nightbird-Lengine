#include "Core/Engine.h"
#include "Core/BackendFactory.h"
#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Input/InputProvider.h"

#include "Core/SpatialObject.h"
#include "Core/Vertex.h"
#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/MeshInstance.h"
#include "Core/Camera.h"
#include "Core/AudioSource.h"
#include "Core/Log.h"

#include "Import/ImportManager.h"
#include "Import/Importer.h"
#include "Import/AssetInfo.h"

#include "Scene/TextSceneWriter.h"
#include "Scene/TextSceneReader.h"

#include "Scene/BinarySceneReader.h"

#include "Cook/CookManager.h"

#include "Load/AudioLoader.h"

using namespace Nightbird;

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

int main()
{
	auto platform = Core::CreatePlatform();
	auto renderer = Core::CreateRenderer();

	Core::Engine engine(std::move(platform), std::move(renderer));

	Editor::ImportManager importManager("Assets");
	importManager.Scan();

	//auto result = importManager.LoadScene("Assets/Models/witch_treehouse.glb");
	//if (auto* witchTreehouse = result.get()->Cast<Core::SpatialObject>())
	//{
	//	witchTreehouse->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	//	witchTreehouse->transform.scale = glm::vec3(0.001f);

	//	engine.GetScene().GetRoot()->AddChild(std::move(result));
	//}
	//else
	//{
	//	Core::Log::Error("Failed to import model");
	//}

	// auto result = importManager.LoadScene("Assets/Models/serena.glb");
	// if (auto* serena = result.get()->Cast<Core::SpatialObject>())
	// {
	// 	serena->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	// 	serena->transform.scale = glm::vec3(0.03f);

	// 	engine.GetScene().GetRoot()->AddChild(std::move(result));
	// }
	// else
	// {
	// 	Core::Log::Error("Failed to import model");
	// }

	Editor::TextSceneReader sceneReader(importManager);
	engine.SetScene(sceneReader.Read("Assets/Scenes/Serena2.ntscene").scene);

	//auto audioUUID = uuids::uuid::from_string("2322e33c-17db-45d1-94e4-846986d0079c");
	//if (!audioUUID)
	//{
		//Core::Log::Error("Invalid audio UUID");
		//return -1;
	//}

	//auto audioSource = std::make_unique<Core::AudioSource>("EPF");
	//audioSource->SetAudioUUID(*audioUUID);
	//audioSource->SetLoop(true);
	//audioSource->SetPlayOnStart(true);
	//engine.GetScene().GetRoot()->AddChild(std::move(audioSource));

	// auto directionalLight = std::make_unique<Core::DirectionalLight>("DirectionalLight");
	// directionalLight->transform.rotation = glm::quat(glm::vec3(glm::radians(-45.0f), glm::radians(45.0f), 0.0f));
	// directionalLight->color = glm::vec3(1.0f, 1.0f, 1.0f);
	// directionalLight->intensity = 1.0f;
	// engine.GetScene().GetRoot()->AddChild(std::move(directionalLight));

	// auto pointLight = std::make_unique<Core::PointLight>("PointLight");
	// pointLight->transform.position = glm::vec3(0.0f, 5.0f, 1.0f);
	// pointLight->color = glm::vec3(1.0f, 0.1f, 0.7f);
	// pointLight->intensity = 1.0f;
	// pointLight->radius = 5.0f;
	// engine.GetScene().GetRoot()->AddChild(std::move(pointLight));

	// auto camera = std::make_unique<Core::Camera>("Camera");
	// camera->transform.position = glm::vec3(0.0f, 1.5f, 5.0f);

	// Core::Camera* cameraPtr = camera.get();

	// engine.GetScene().GetRoot()->AddChild(std::move(camera));
	// engine.GetScene().SetActiveCamera(cameraPtr);

	//Editor::TextSceneWriter sceneWriter;
	//sceneWriter.Write(engine.GetScene(), "Serena", GenerateUUID(), "Assets/Scenes/Serena2.ntscene");

	Editor::CookManager cookManager("Cooked", importManager);
	cookManager.Cook("Assets/Scenes/Serena2.ntscene", Editor::CookTarget::Desktop);
	cookManager.Cook("Assets/Scenes/Serena2.ntscene", Editor::CookTarget::WiiU);
	cookManager.Cook("Assets/Scenes/Serena2.ntscene", Editor::CookTarget::N3DS);

	engine.Initialize();
	engine.RunLoop();
	engine.Shutdown();

	return 0;
}
