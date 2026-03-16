#include "Core/Scene.h"

#include "Core/SceneObject.h"
#include "Core/MeshInstance.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	Scene::Scene()
	{
		m_Root = std::make_unique<SceneObject>("Root");
	}
	
	void Scene::Update(float delta)
	{
		for (const auto& child : m_Root->GetChildren())
		{
			child->Tick(delta);
		}
	}

	SceneObject* Scene::GetRoot()
	{
		return m_Root.get();
	}

	Camera* Scene::GetActiveCamera() const
	{
		return m_ActiveCamera;
	}

	void Scene::SetActiveCamera(Camera* camera)
	{
		m_ActiveCamera = camera;
	}

	std::vector<Renderable> Scene::CollectRenderables() const
	{
		std::vector<Renderable> renderables;
		CollectRenderablesRecursive(m_Root.get(), renderables);
		return renderables;
	}

	std::vector<DirectionalLight*> Scene::CollectDirectionalLights() const
	{
		std::vector<DirectionalLight*> directionalLights;
		CollectDirectionalLightsRecursive(m_Root.get(), directionalLights);
		return directionalLights;
	}

	std::vector<PointLight*> Scene::CollectPointLights() const
	{
		std::vector<PointLight*> pointLights;
		CollectPointLightsRecursive(m_Root.get(), pointLights);
		return pointLights;
	}

	void Scene::CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const
	{
		if (!object)
			return;
		
		if (auto* meshInstance = object->Cast<MeshInstance>())
		{
			const Mesh* mesh = meshInstance->GetMesh().get();
			for (size_t i = 0; i < mesh->GetPrimitiveCount(); i++)
			{
				Renderable renderable;
				renderable.primitive = &mesh->GetPrimitives()[i];
				renderable.transform = meshInstance->GetWorldMatrix();
				renderables.push_back(renderable);
			}
		}

		for (const auto& child : object->GetChildren())
			CollectRenderablesRecursive(child.get(), renderables);
	}

	void Scene::CollectDirectionalLightsRecursive(SceneObject* object, std::vector<DirectionalLight*>& directionalLights) const
	{
		if (!object)
			return;

		if (auto* directionalLight = object->Cast<DirectionalLight>())
			directionalLights.push_back(directionalLight);

		for (const auto& child : object->GetChildren())
			CollectDirectionalLightsRecursive(child.get(), directionalLights);
	}
	
	void Scene::CollectPointLightsRecursive(SceneObject* object, std::vector<PointLight*>& pointLights) const
	{
		if (!object)
			return;

		if (auto* pointLight = object->Cast<PointLight>())
			pointLights.push_back(pointLight);

		for (const auto& child : object->GetChildren())
			CollectPointLightsRecursive(child.get(), pointLights);
	}
}
