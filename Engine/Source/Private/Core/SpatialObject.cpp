#include "Core/SpatialObject.h"

namespace Nightbird::Core
{
	glm::mat4 SpatialObject::GetLocalMatrix() const
	{
		return m_Transform.GetLocalMatrix();
	}

	glm::mat4 SpatialObject::GetWorldMatrix() const
	{
		if (m_Parent)
		{
			if (auto* spatialParent = Cast<SpatialObject>(m_Parent))
				return spatialParent->GetWorldMatrix() * GetLocalMatrix();
		}

		return GetLocalMatrix();
	}
}
