#pragma once

#include "Core/TypeInfo.h"
#include "Core/Reflection.h"

#include "Core/AssetLoader.h"

#include <uuid.h>

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace Nightbird::Core
{
	class Engine;
	class Scene;

	class SceneObject
	{
	public:
		NB_TYPE_BASE()
		NB_REFLECTION_FRIEND

		SceneObject();
		virtual ~SceneObject() = default;

		const std::string& GetName() const;
		void SetName(std::string name);

		Engine* GetEngine() const;
		void SetScene(Scene* scene);

		SceneObject* GetParent() const;
		void SetParent(SceneObject* newParent);

		const std::vector<std::unique_ptr<SceneObject>>& GetChildren() const;
		std::vector<std::unique_ptr<SceneObject>>& GetChildren();
		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);

		bool HasSourceScene() const;
		const std::optional<uuids::uuid>& GetSourceSceneUUID() const;
		void SetSourceSceneUUID(const uuids::uuid& uuid);

		void EnterSceneRecursive();

		virtual void LoadAssets(AssetLoader& assetLoader) {}
		virtual void EnterScene();
		virtual void Tick(float delta);

	protected:
		std::string m_Name;
		Scene* m_Scene = nullptr;
		SceneObject* m_Parent = nullptr;
		std::vector<std::unique_ptr<SceneObject>> m_Children;

		std::optional<uuids::uuid> m_SourceSceneUUID;
	};
}
