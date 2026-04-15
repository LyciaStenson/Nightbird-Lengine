#include "Core/PointLight.h"

namespace Nightbird::Core
{
	NB_REFLECT_TYPE(PointLight, NB_PARENT(SceneObject), NB_FACTORY(PointLight),
		NB_FIELD(m_Color, Vector3),
		NB_FIELD(m_Intensity, Float),
		NB_FIELD(m_Radius, Float)
	)
}
