#include "Core/DirectionalLight.h"

namespace Nightbird::Core
{
	NB_REFLECT(DirectionalLight, NB_PARENT(SceneObject), NB_FACTORY(DirectionalLight),
		NB_FIELD(m_Color),
		NB_FIELD(m_Intensity)
	)
}
