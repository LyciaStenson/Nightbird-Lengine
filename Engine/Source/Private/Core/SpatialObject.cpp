#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	const TypeInfo SpatialObject::s_TypeInfo = { "SpatialObject", &SceneObject::s_TypeInfo };

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
