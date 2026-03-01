#pragma once

#include "Core/Renderable.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"

#include <memory>
#include <vector>

namespace Nightbird::Core
{
	class MeshInstance;
	class SceneObject;
	class Camera;

	class Scene
	{
	public:
		Scene();

		void Update(float delta);

		SceneObject* GetRoot();

		Camera* GetActiveCamera() const;
		void SetActiveCamera(Camera* camera);

		std::vector<Renderable> CollectRenderables() const;
		std::vector<DirectionalLight*> CollectDirectionalLights() const;
		std::vector<PointLight*> CollectPointLights() const;

	private:
		std::unique_ptr<SceneObject> m_Root;

		Camera* m_ActiveCamera = nullptr;

		void CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const;
		void CollectDirectionalLightsRecursive(SceneObject* object, std::vector<DirectionalLight*>& directionalLights) const;
		void CollectPointLightsRecursive(SceneObject* object, std::vector<PointLight*>& pointLights) const;
	};
}