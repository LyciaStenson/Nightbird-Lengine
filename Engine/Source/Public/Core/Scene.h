#pragma once

#include "Core/Renderable.h"

#include <memory>
#include <vector>

namespace Nightbird::Core
{
	class MeshInstance;
	class SceneObject;

	class Scene
	{
	public:
		Scene();

		SceneObject* GetRoot();

		void Update(float delta);

		std::vector<Renderable> CollectRenderables() const;

	private:
		std::unique_ptr<SceneObject> m_Root;

		void CollectRenderablesRecursive(SceneObject* object, std::vector<Renderable>& renderables) const;
	};
}