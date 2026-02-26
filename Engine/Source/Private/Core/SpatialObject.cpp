#include "Core/SpatialObject.h"

namespace Nightbird
{
	glm::mat4 SpatialObject::GetLocalMatrix() const
	{
		return transform.GetLocalMatrix();
	}

	glm::mat4 SpatialObject::GetWorldMatrix() const
	{
		if (m_Parent)
		{
			if (auto* spatialParent = dynamic_cast<SpatialObject*>(m_Parent))
				return spatialParent->GetLocalMatrix() * GetLocalMatrix();
		}

		return GetLocalMatrix();
	}
}