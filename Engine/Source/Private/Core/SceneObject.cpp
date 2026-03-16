#include "Core/SceneObject.h"

namespace Nightbird::Core
{
	const TypeInfo SceneObject::s_TypeInfo = { "SceneObject", nullptr };

	SceneObject::SceneObject(const std::string& name)
		: m_Name(name)
	{

	}

	const std::string& SceneObject::GetName() const
	{
		return m_Name;
	}

	void SceneObject::SetParent(SceneObject* parent)
	{
		m_Parent = parent;
	}

	SceneObject* SceneObject::GetParent() const
	{
		return m_Parent;
	}

	const std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren() const
	{
		return m_Children;
	}

	std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren()
	{
		return m_Children;
	}

	void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
	{
		child->SetParent(this);
		m_Children.push_back(std::move(child));
	}

	std::unique_ptr<SceneObject> SceneObject::DetachChild(SceneObject* child)
	{
		for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
		{
			if (it->get() == child)
			{
				child->SetParent(nullptr);
				auto detatched = std::move(*it);
				m_Children.erase(it);
				return detatched;
			}
		}

		return nullptr;
	}

	bool SceneObject::HasSourceScene() const
	{
		return m_SourceSceneUUID.has_value();
	}

	const std::optional<uuids::uuid>& SceneObject::GetSourceSceneUUID() const
	{
		return m_SourceSceneUUID;
	}

	void SceneObject::SetSourceSceneUUID(const uuids::uuid& uuid)
	{
		m_SourceSceneUUID = uuid;
	}

	void SceneObject::EnterScene()
	{

	}

	void SceneObject::Tick(float delta)
	{

	}
}
