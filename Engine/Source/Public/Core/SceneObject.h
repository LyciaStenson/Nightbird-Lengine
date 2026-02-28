#pragma once

#include <string>
#include <vector>
#include <memory>

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
		void AddChild(std::unique_ptr<SceneObject> child);
		std::unique_ptr<SceneObject> DetachChild(SceneObject* child);

		virtual void EnterScene();
		virtual void Tick(float delta);

	protected:
		std::string m_Name;
		SceneObject* m_Parent = nullptr;
		std::vector<std::unique_ptr<SceneObject>> m_Children;
	};
}