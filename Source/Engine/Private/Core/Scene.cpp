#include "Core/Scene.h"

#include <iostream>

#include "Vulkan/Device.h"
#include "Vulkan/StorageBuffer.h"
#include "Vulkan/GlobalDescriptorSetManager.h"
#include "Core/SceneObject.h"
#include "Core/PrefabInstance.h"
#include "Core/MeshInstance.h"
#include "Core/ModelManager.h"
#include "Core/Mesh.h"
#include "Core/Camera.h"
#include "Core/CameraUBO.h"
#include "Core/DirectionalLight.h"
#include "Core/DirectionalLightData.h"
#include "Core/PointLight.h"
#include "Core/PointLightData.h"
#include "Core/Transform.h"
#include "Core/Model.h"

#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

namespace Nightbird
{
	Scene::Scene(VulkanDevice* device, ModelManager* modelManager, GlobalDescriptorSetManager* globalDescriptorSetManager, VkDescriptorPool descriptorPool)
		: device(device), modelManager(modelManager), globalDescriptorSetManager(globalDescriptorSetManager), descriptorPool(descriptorPool)
	{
		rootObject = std::make_unique<SceneObject>("Root");
	}

	Scene::~Scene()
	{

	}

	const SceneObject* Scene::GetRootObject() const
	{
		return rootObject.get();
	}
	
	SceneObject* Scene::GetRootObject()
	{
		return rootObject.get();
	}

	std::vector<SceneObject*> Scene::GetAllObjects()
	{
		std::vector<SceneObject*> allObjects;
		GetAllObjectsRecursive(rootObject.get(), allObjects);
		return allObjects;
	}

	void Scene::GetAllObjectsRecursive(SceneObject* root, std::vector<SceneObject*>& allObjects)
	{
		allObjects.push_back(root);
		for (const auto& child : root->GetChildren())
		{
			GetAllObjectsRecursive(child.get(), allObjects);
		}
	}

	Camera* Scene::GetMainCamera()
	{
		if (mainCamera)
			return mainCamera;
		else
		{
			std::vector<SceneObject*> objects = GetAllObjects();
			for (SceneObject* object : objects)
			{
				if (Camera* camera = dynamic_cast<Camera*>(object))
					return camera;
			}
			return nullptr;
		}
	}

	void Scene::SetMainCamera(Camera* camera)
	{
		mainCamera = camera;
	}

	bool Scene::LoadSceneJSON(const std::string& path)
	{
		std::ifstream is(path);
		if (!is.is_open())
		{
			std::cerr << "Failed to open scene for reading at " << path << std::endl;
			return false;
		}

		nlohmann::json json;
		is >> json;
		rootObject->Deserialize(json);

		std::vector<SceneObject*> allObjects = GetAllObjects();
		for (SceneObject* object : allObjects)
		{
			//object->transform.eulerCache = glm::degrees(glm::eulerAngles(object->transform.rotation));
			if (auto* prefab = dynamic_cast<PrefabInstance*>(object))
			{
				modelManager->LoadModelAsync(prefab->GetPrefabPath(), [this, prefab](std::shared_ptr<Model> model)
					{
						InstantiateModel(prefab);
					});
			}
		}

		return true;
	}
	
	bool Scene::SaveSceneJSON(const std::string& path) const
	{
		nlohmann::json json;
		rootObject->Serialize(json);
		
		std::ofstream os(path);
		if (!os.is_open())
		{
			std::cerr << "Failed to open scene for writing at " << path << std::endl;
			return false;
		}
		
		os << json.dump(4);
		
		return true;
	}

	bool Scene::LoadSceneBIN(const std::string& path)
	{
		std::ifstream is(path, std::ios::binary);
		if (!is.is_open())
		{
			std::cerr << "Failed to open scene for reading at " << path << std::endl;
			return false;
		}

		std::vector<SceneObject*> allObjects = GetAllObjects();
		for (SceneObject* object : allObjects)
		{
			if (auto* prefab = dynamic_cast<PrefabInstance*>(object))
			{
				modelManager->LoadModel(prefab->GetPrefabPath());
				InstantiateModel(prefab);
			}
		}

		return true;
	}

	bool Scene::SaveSceneBIN(const std::string& path) const
	{
		std::ofstream os(path, std::ios::binary);
		if (!os.is_open())
		{
			std::cerr << "Failed to open scene for writing at " << path << std::endl;
			return false;
		}
		
		return true;
	}
	
	void Scene::AddSceneObject(std::unique_ptr<SceneObject> object, SceneObject* parent)
	{
		SceneObject* rawObject = object.get();
		
		if (parent)
			parent->AddChild(std::move(object));
		else
			rootObject->AddChild(std::move(object));
		
		rawObject->EnterScene();
	}

	SceneObject* Scene::CreateSceneObject(const std::string& name, SceneObject* parent)
	{
		if (!parent)
			parent = rootObject.get();

		std::string instanceName = name;
		//int counter = 1;
		//while (objectNames.count(instanceName))
		//{
			//instanceName = name + std::to_string(counter);
			//++counter;
		//}
		//objectNames.insert(instanceName);

		std::unique_ptr<SceneObject> object(new SceneObject(instanceName));
		object->SetParent(parent);

		SceneObject* objectPtr = object.get();

		AddSceneObject(std::move(object), parent);

		return objectPtr;
	}

	SpatialObject* Scene::CreateSpatialObject(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent)
	{
		if (!parent)
			parent = rootObject.get();

		std::string instanceName = name;
		//int counter = 1;
		//while (objectNames.count(instanceName))
		//{
			//instanceName = name + std::to_string(counter);
			//++counter;
		//}
		//objectNames.insert(instanceName);

		std::unique_ptr<SceneObject> object(new SpatialObject(instanceName));
		SpatialObject* objectPtr = static_cast<SpatialObject*>(object.get());

		objectPtr->transform.position = position;
		objectPtr->transform.rotation = rotation;
		objectPtr->transform.scale = scale;
		objectPtr->SetParent(parent);

		AddSceneObject(std::move(object), parent);

		return objectPtr;
	}
	
	PrefabInstance* Scene::CreatePrefabInstance(const std::string& name, const std::string& path, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent)
	{
		if (!parent)
			parent = rootObject.get();

		std::string instanceName = name;
		//int counter = 1;
		//while (objectNames.count(instanceName))
		//{
			//instanceName = name + std::to_string(counter);
			//++counter;
		//}
		//objectNames.insert(instanceName);

		std::unique_ptr<SceneObject> object(new PrefabInstance(instanceName, path));
		PrefabInstance* prefab = static_cast<PrefabInstance*>(object.get());

		prefab->transform.position = position;
		prefab->transform.rotation = rotation;
		prefab->transform.scale = scale;
		prefab->SetParent(parent);
		
		AddSceneObject(std::move(object), parent);

		return prefab;
	}

	MeshInstance* Scene::CreateMeshInstance(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, SceneObject* parent, std::shared_ptr<Mesh> mesh)
	{
		if (!parent)
			parent = rootObject.get();

		std::string instanceName = name;
		//int counter = 1;
		//while (objectNames.count(instanceName))
		//{
			//instanceName = name + std::to_string(counter);
			//++counter;
		//}
		//objectNames.insert(instanceName);

		std::unique_ptr<SceneObject> object(new MeshInstance(instanceName, mesh, device, descriptorPool));
		MeshInstance* meshInstance = static_cast<MeshInstance*>(object.get());

		meshInstance->transform.position = position;
		meshInstance->transform.rotation = rotation;
		meshInstance->transform.scale = scale;
		meshInstance->SetParent(parent);
		
		AddSceneObject(std::move(object), parent);

		return meshInstance;
	}

	void Scene::InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node, SceneObject* parent)
	{
		fastgltf::math::fvec3 gltfTranslation(0.0f, 0.0f, 0.0f);
		fastgltf::math::fquat gltfRotation;
		fastgltf::math::fvec3 gltfScale(1.0f, 1.0f, 1.0f);

		if (auto* transform = std::get_if<fastgltf::math::fmat4x4>(&node.transform))
		{
			fastgltf::math::decomposeTransformMatrix(*transform, gltfScale, gltfRotation, gltfTranslation);
		}
		else if (auto* trs = std::get_if<fastgltf::TRS>(&node.transform))
		{
			gltfTranslation = trs->translation;
			gltfRotation = trs->rotation;
			gltfScale = trs->scale;
		}

		glm::vec3 position = glm::vec3(gltfTranslation.x(), gltfTranslation.y(), gltfTranslation.z());
		glm::quat rotation = glm::quat(gltfRotation.w(), gltfRotation.x(), gltfRotation.y(), gltfRotation.z());
		glm::vec3 scale = glm::vec3(gltfScale.x(), gltfScale.y(), gltfScale.z());

		SceneObject* object = nullptr;
		
		if (node.meshIndex.has_value())
		{
			if (node.meshIndex.value() < model->meshes.size())
			{
				object = CreateMeshInstance(node.name.c_str(), position, rotation, scale, parent, model->meshes[node.meshIndex.value()]);
			}
		}
		else
		{
			object = CreateSpatialObject(node.name.c_str(), position, rotation, scale, parent);
		}

		for (const auto& childNodeIndex : node.children)
		{
			const auto& childNode = model->gltfAsset.nodes[childNodeIndex];
			if (object)
				InstantiateModelNode(model, childNode, object);
		}
	}

	void Scene::InstantiateModel(PrefabInstance* prefab)
	{
		const auto& model = modelManager->GetModel(prefab->GetPrefabPath());

		if (!model)
		{
			std::cout << "Model at " << prefab->GetPrefabPath() << " not found. Make sure model is loaded first." << std::endl;
			return;
		}

		const fastgltf::Scene& gltfScene = model->gltfAsset.scenes[0];

		for (size_t rootNodeIndex : gltfScene.nodeIndices)
		{
			InstantiateModelNode(model, model->gltfAsset.nodes[rootNodeIndex], prefab);
		}
	}

	PrefabInstance* Scene::InstantiateModel(const std::string& path, const Transform& transform)
	{
		const auto& model = modelManager->GetModel(path);

		if (!model)
		{
			std::cout << "Model at " << path << " not found. Make sure model is loaded first." << std::endl;
			return nullptr;
		}

		const fastgltf::Scene& gltfScene = model->gltfAsset.scenes[0];

		PrefabInstance* root = CreatePrefabInstance("Model", path, transform.position, transform.rotation, transform.scale, nullptr);

		for (size_t rootNodeIndex : gltfScene.nodeIndices)
		{
			InstantiateModelNode(model, model->gltfAsset.nodes[rootNodeIndex], root);
		}

		return root;
	}

	SceneObject* Scene::FindObject(const std::string& path, SceneObject* root)
	{
		size_t start = 0;
		size_t end = path.find('/');

		SceneObject* node = root ? root : rootObject.get();

		while (true)
		{
			std::string token = path.substr(start, end - start);

			bool found = false;
			for (const auto& child : node->GetChildren())
			{
				if (child->GetName() == token)
				{
					node = child.get();
					found = true;
					break;
				}
			}

			if (!found)
				return nullptr;

			if (end == std::string::npos)
				break;

			start = end + 1;
			end = path.find('/', start);
		}

		return node;
	}

	void Scene::Update(float delta)
	{
		for (auto& object : GetAllObjects())
		{
			object->Tick(delta);
		}
	}

	void Scene::UpdateBuffers(int currentFrame, VkExtent2D swapChainExtent)
	{
		std::vector<DirectionalLightData> directionalLightData;
		std::vector<PointLightData> pointLightData;

		UpdateBuffersRecursive(currentFrame, swapChainExtent, rootObject.get(), directionalLightData, pointLightData);

		globalDescriptorSetManager->UpdateDirectionalLights(currentFrame, directionalLightData);
		globalDescriptorSetManager->UpdatePointLights(currentFrame, pointLightData);
	}

	void Scene::UpdateBuffersRecursive(int currentFrame, VkExtent2D swapChainExtent, SceneObject* object, std::vector<DirectionalLightData>& directionalLightData, std::vector<PointLightData>& pointLightData)
	{
		if (!object)
			return;

		if (auto* meshInstance = dynamic_cast<MeshInstance*>(object))
		{
			meshInstance->UpdateUniformBuffer(currentFrame);
		}
		else if (auto* directionalLight = dynamic_cast<DirectionalLight*>(object))
		{
			directionalLightData.push_back(directionalLight->GetData());
		}
		else if (auto* pointLight = dynamic_cast<PointLight*>(object))
		{
			pointLightData.push_back(pointLight->GetData());
		}

		for (const auto& child : object->GetChildren())
			UpdateBuffersRecursive(currentFrame, swapChainExtent, child.get(), directionalLightData, pointLightData);
	}
}