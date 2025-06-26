#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <string>

#include <volk.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fastgltf/core.hpp>
#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanStorageBuffer;
	class GlobalDescriptorSetManager;
	class SceneObject;
	class MeshInstance;
	class ModelManager;
	class Mesh;
	class Camera;
	class PointLight;
	class Transform;
	struct Model;

	class Scene
	{
	public:
		Scene(VulkanDevice* device, ModelManager* modelManager, GlobalDescriptorSetManager* globalDescriptorSetManager, VkDescriptorSetLayout cameraDescriptorSetLayout, VkDescriptorPool descriptorPool);
		~Scene();

		const std::vector<std::unique_ptr<SceneObject>>& GetObjects() const;
		std::vector<std::unique_ptr<SceneObject>>& GetObjectsMutable();

		Camera* GetMainCamera() const;
		void SetMainCamera(Camera* camera);
		
		SceneObject* CreateSceneObject(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent = nullptr);
		Camera* CreateCamera(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent = nullptr);
		PointLight* CreatePointLight(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent = nullptr);

		SceneObject* InstantiateModel(const std::string& name, const Transform& transform);
		
		void UpdateBuffers(int currentFrame, VkExtent2D swapChainExtent);

	private:
		VulkanDevice* device;
		
		VkDescriptorSetLayout cameraDescriptorSetLayout;

		VkDescriptorPool descriptorPool;

		GlobalDescriptorSetManager* globalDescriptorSetManager;
		ModelManager* modelManager;

		std::vector<std::unique_ptr<SceneObject>> objects;
		std::unordered_set<std::string> objectNames;
		
		Camera* mainCamera = nullptr;

		void InstantiateModelNode(const std::shared_ptr<Model>& model, const fastgltf::Node& node, Transform* parent);
		
		MeshInstance* CreateMeshInstance(const std::string& name, const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale, Transform* parent, std::shared_ptr<Mesh> mesh);

		//void CreateLightBuffer(uint32_t maxLights);
		//void UpdateLightBuffer();
	};
}