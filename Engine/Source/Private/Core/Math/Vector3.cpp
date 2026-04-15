#include "Core/Math/Vector3.h"

namespace Nightbird::Core
{
	NB_REFLECT(Vector3, NB_NO_PARENT, NB_FACTORY(Vector3),
		NB_FIELD(x, Float),
		NB_FIELD(y, Float),
		NB_FIELD(z, Float)
	)
}
