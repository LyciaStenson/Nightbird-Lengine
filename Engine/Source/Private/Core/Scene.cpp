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

	void Scene::CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const
	{
		if (!object)
			return;
		
		if (auto* meshInstance = dynamic_cast<MeshInstance*>(object))
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
}