#include <Core/SceneObject.h>

#include <iostream>

namespace Nightbird
{
	SceneObject::SceneObject(const char* name)
		: name(name ? name : "")
	{

	}

	SceneObject::SceneObject(const std::string& name)
		: name(name)
	{

	}
	
	const std::string& SceneObject::GetName() const
	{
		return name;
	}

	std::string SceneObject::GetPath() const
	{
		if (!parent || !parent->parent)
			return name;

		return parent->GetPath() + '/' + name;
	}

	void SceneObject::SetParent(SceneObject* newParent)
	{
		if (parent == newParent)
			return;
		
		std::unique_ptr<SceneObject> uniquePtr = nullptr;
		if (parent)
			uniquePtr = parent->DetachChild(this);

		parent = newParent;

		if (newParent && uniquePtr)
			newParent->AddChild(std::move(uniquePtr));
	}

	SceneObject* SceneObject::GetParent() const
	{
		return parent;
	}

	const std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren() const
	{
		return children;
	}

	glm::mat4 SceneObject::GetLocalMatrix() const
	{
		return transform.GetLocalMatrix();
	}

	glm::mat4 SceneObject::GetWorldMatrix() const
	{
		if (parent)
			return parent->GetWorldMatrix() * transform.GetLocalMatrix();
		else
			return transform.GetLocalMatrix();
	}

	void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
	{
		child->parent = this;
		children.push_back(std::move(child));
	}

	std::unique_ptr<SceneObject> SceneObject::DetachChild(SceneObject* child)
	{
		auto it = std::find_if(children.begin(), children.end(),
			[child](const std::unique_ptr<SceneObject>& existingChild)
			{
				return existingChild.get() == child;
			}
		);

		if (it != children.end())
		{
			std::unique_ptr<SceneObject> detachedChild = std::move(*it);
			children.erase(it);
			detachedChild->parent = nullptr;
			return detachedChild;
		}

		return nullptr;
	}
	
	void SceneObject::Serialize(json& out) const
	{
		//out["type"] = GetTypeName();
		out["name"] = name;
		out["transform"] = transform;
		out["children"] = json::array();
		for (const auto& child : children)
		{
			json childJson;
			child->Serialize(childJson);
			out["children"].push_back(childJson);
		}
	}

	void SceneObject::Deserialize(const json& in)
	{
		name = in.at("name").get<std::string>();
		in.at("transform").get_to(transform);

		children.clear();
		for (const auto& childJson : in.at("children"))
		{
			//std::string childType = childJson.at("type").get<std::string>();
			//std::string childName = childJson.at("name").get<std::string>();

			//SceneObject* object = nullptr;

			//std::vector<SceneObjectDescriptor>& registry = GetSceneObjectRegistry();

			//for (const auto& desc : GetSceneObjectRegistry())
			//{
				//if (childType == desc.typeName)
				//{
					//object = desc.create(childName.c_str());
					//break;
				//}
			//}

			//if (!object)
				//std::cerr << "Unknown SceneObject type " << childType << std::endl;

			//std::unique_ptr<SceneObject, SceneObjectDeleter> child(object, SceneObjectDeleter());
			//child->SetParent(this);
			//child->Deserialize(childJson);
			//children.push_back(std::move(child));
		}
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::SceneObject>("SceneObject")
	.constructor<std::string>()
	.property("name", &Nightbird::SceneObject::name)
	.property("transform", &Nightbird::SceneObject::transform);

	rttr::registration::method("CreateSceneObject", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::SceneObject(name);
	});
}