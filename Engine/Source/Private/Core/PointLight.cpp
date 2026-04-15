#include "Core/PointLight.h"

namespace Nightbird::Core
{
	NB_TYPE_IMPL(PointLight, SceneObject)
		NB_FIELD(m_Color, Vector3)
		NB_FIELD(m_Intensity, Float)
		NB_FIELD(m_Radius, Float)
	NB_TYPE_IMPL_END()
}
