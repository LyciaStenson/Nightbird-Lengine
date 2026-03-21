#include "Core/SceneObject.h"

#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	NB_OBJECT_BASE_IMPL(SceneObject)

	SceneObject::SceneObject(const std::string& name)
		: m_Name(name)
	{

	}

	const std::string& SceneObject::GetName() const
	{
		return m_Name;
	}

	Engine* SceneObject::GetEngine() const
	{
		return m_Scene ? m_Scene->GetEngine() : nullptr;
	}

	void SceneObject::SetScene(Scene* scene)
	{
		m_Scene = scene;
		for (auto& child : m_Children)
			child->SetScene(scene);
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
		child->SetScene(m_Scene);
		child->SetParent(this);
		m_Children.push_back(std::move(child));

		if (m_Scene && m_Scene->GetEngine())
			m_Children.back()->EnterSceneRecursive();
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

	void SceneObject::EnterSceneRecursive()
	{
		EnterScene();
		for (const auto& child : m_Children)
			child->EnterSceneRecursive();
	}

	void SceneObject::EnterScene()
	{

	}

	void SceneObject::Tick(float delta)
	{

	}
}
