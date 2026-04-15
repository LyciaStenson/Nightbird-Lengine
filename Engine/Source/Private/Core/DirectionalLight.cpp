#include "Core/DirectionalLight.h"

namespace Nightbird::Core
{
	NB_REFLECT(DirectionalLight, NB_PARENT(SceneObject), NB_FACTORY(DirectionalLight),
		NB_FIELD(m_Color, Vector3),
		NB_FIELD(m_Intensity, Float)
	)
}
