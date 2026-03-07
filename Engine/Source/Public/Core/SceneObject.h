#pragma once

#include <uuid.h>

#include <string>
#include <vector>
#include <memory>
#include <optional>

namespace Nightbird::Core
{
	class SceneObject
	{
	public:
		SceneObject(const std::string& name);
		virtual ~SceneObject() = default;

		const std::string& GetName() const;

		void SetParent(SceneObject* parent);
		SceneObject* GetParent() const;
		
		const std::vector<std::unique_ptr<SceneObject>>& GetChildren() const;
		std::vector<std::unique_ptr<SceneObject>>& GetChildren();
		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);
		
		bool HasSourceScene() const;
		const std::optional<uuids::uuid>& GetSourceSceneUUID() const;
		void SetSourceSceneUUID(const uuids::uuid& uuid);

		virtual void EnterScene();
		virtual void Tick(float delta);

	protected:
		std::string m_Name;
		SceneObject* m_Parent = nullptr;
		std::vector<std::unique_ptr<SceneObject>> m_Children;

		std::optional<uuids::uuid> m_SourceSceneUUID;
	};
}