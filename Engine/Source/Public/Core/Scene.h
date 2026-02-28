#pragma once

#include "Core/Renderable.h"

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

	private:
		std::unique_ptr<SceneObject> m_Root;

		Camera* m_ActiveCamera = nullptr;

		void CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const;
	};
}