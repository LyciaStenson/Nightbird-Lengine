#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	NB_OBJECT_IMPL(SpatialObject, SceneObject);

	glm::mat4 SpatialObject::GetLocalMatrix() const
	{
		return transform.GetLocalMatrix();
	}

	glm::mat4 SpatialObject::GetWorldMatrix() const
	{
		if (m_Parent)
		{
			if (auto* spatialParent = m_Parent->Cast<SpatialObject>())
				return spatialParent->GetWorldMatrix() * GetLocalMatrix();
		}

		return GetLocalMatrix();
	}
}
