#include "Core/SpatialObject.h"

namespace Nightbird
{
	glm::mat4 SpatialObject::GetLocalMatrix() const
	{
		return transform.GetLocalMatrix();
	}

	glm::mat4 SpatialObject::GetWorldMatrix() const
	{
		if (parent)
		{
			if (SpatialObject* spatialParent = dynamic_cast<SpatialObject*>(parent))
				return spatialParent->GetWorldMatrix() * transform.GetLocalMatrix();
		}
		return transform.GetLocalMatrix();
	}
}

RTTR_REGISTRATION
{
	rttr::registration::class_<Nightbird::SpatialObject>("SpatialObject")
	.constructor<std::string>()
	.property("Transform", &Nightbird::SpatialObject::transform);

	rttr::registration::method("CreateSpatialObject", [](const std::string& name) -> Nightbird::SceneObject*
	{
		return new Nightbird::SpatialObject(name);
	});
}