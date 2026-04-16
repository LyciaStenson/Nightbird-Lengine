#include "Core/PointLight.h"

namespace Nightbird::Core
{
	NB_REFLECT(PointLight, NB_PARENT(SceneObject), NB_FACTORY(PointLight),
		NB_FIELD(m_Color),
		NB_FIELD(m_Intensity),
		NB_FIELD(m_Radius)
	)
}
