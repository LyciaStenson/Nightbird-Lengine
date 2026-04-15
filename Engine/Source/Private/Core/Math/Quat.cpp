#include "Core/Math/Quat.h"

namespace Nightbird::Core
{
	NB_REFLECT_TYPE(Quat, NB_NO_PARENT, NB_FACTORY(Quat),
		NB_FIELD(x, Float),
		NB_FIELD(y, Float),
		NB_FIELD(z, Float),
		NB_FIELD(w, Float)
	)
}
