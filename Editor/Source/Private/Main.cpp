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

#include "Import/ImportManager.h"
#include "Import/Importer.h"
#include "Import/AssetInfo.h"

using namespace Nightbird;

int main()
{
	auto platform = Core::CreatePlatform();
	auto renderer = Core::CreateRenderer();

	Core::Engine engine(std::move(platform), std::move(renderer));

	Editor::ImportManager importManager("Assets");
	importManager.Scan();

	auto result = importManager.Import("Assets/Models/witch_treehouse.glb");
	if (result)
	{
		result->transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
		result->transform.scale = glm::vec3(0.001f);
		engine.GetScene().GetRoot()->AddChild(std::move(result));
	}
	
	auto camera = std::make_unique<Core::Camera>("Camera");
	camera->transform.position = glm::vec3(0.0f, 1.5f, 5.0f);

	Core::Camera* cameraPtr = camera.get();
	
	engine.GetScene().GetRoot()->AddChild(std::move(camera));
	engine.GetScene().SetActiveCamera(cameraPtr);

	engine.Run();

	return 0;
}