#include "Core/DirectionalLight.h"

namespace Nightbird::Core
{
	NB_TYPE_IMPL(DirectionalLight, SceneObject)
		NB_FIELD(m_Color, Vector3)
		NB_FIELD(m_Intensity, Float)
	NB_TYPE_IMPL_END()
}
