#pragma once

#include <vector>
#include <memory>
#include <string>

#include <Core/Transform.h>

#include <nlohmann/json.hpp>
#include <Core/TransformSerialization.h>

#include <rttr/type>
#include <rttr/registration.h>
#include <rttr/registration_friend.h>

//using DeleteCustomObjectFunc = void (*)(void*);
//extern DeleteCustomObjectFunc g_DeleteCustomObject;

using json = nlohmann::json;

namespace Nightbird
{
	struct SceneObjectDeleter
	{
		void operator()(SceneObject* object) const;
	};

	class SceneObject
	{
	public:
		SceneObject() = default;
		SceneObject(const char* name);
		SceneObject(const std::string& name);
		virtual ~SceneObject();

		const std::string& GetName() const;
		std::string GetPath() const;
		
		void SetParent(SceneObject* transform);
		SceneObject* GetParent() const;
		
		const std::vector<std::unique_ptr<SceneObject, SceneObjectDeleter>>& GetChildren() const;

		glm::mat4 GetLocalMatrix() const;
		glm::mat4 GetWorldMatrix() const;

		void AddChild(std::unique_ptr<SceneObject, SceneObjectDeleter> child);
		std::unique_ptr<SceneObject, SceneObjectDeleter> DetachChild(SceneObject* child);
		
		virtual const char* GetTypeName() const { return "SceneObject"; }
		
		virtual void Serialize(json& out) const;
		virtual void Deserialize(const json& in);
		
		Transform transform;

		SceneObject* parent = nullptr;

		virtual bool IsCustomObject() const { return false; }
		
		RTTR_ENABLE()
		RTTR_REGISTRATION_FRIEND

	protected:
		std::string name;
		
		std::vector<std::unique_ptr<SceneObject, SceneObjectDeleter>> children;
	};
}