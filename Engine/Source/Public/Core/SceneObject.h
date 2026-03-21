#pragma once

#include "Core/TypeInfo.h"

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
		NB_OBJECT_BASE(SceneObject)

		template<typename T>
		bool IsA() const { return GetTypeInfo()->IsA(&T::s_TypeInfo); }

		template<typename T>
		T* Cast()
		{
			if (IsA<T>())
				return static_cast<T*>(this);
			return nullptr;
		}

		template<typename T>
		const T* Cast() const
		{
			if (IsA<T>())
				return static_cast<const T*>(this);
			return nullptr;
		}

		SceneObject(const std::string& name);
		virtual ~SceneObject() = default;

		const std::string& GetName() const;

		Engine* GetEngine() const;
		void SetScene(Scene* scene);

		void SetParent(SceneObject* parent);
		SceneObject* GetParent() const;
		
		const std::vector<std::unique_ptr<SceneObject>>& GetChildren() const;
		std::vector<std::unique_ptr<SceneObject>>& GetChildren();
		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);
		
		bool HasSourceScene() const;
		const std::optional<uuids::uuid>& GetSourceSceneUUID() const;
		void SetSourceSceneUUID(const uuids::uuid& uuid);

		void EnterSceneRecursive();

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
