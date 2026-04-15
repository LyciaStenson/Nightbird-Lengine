#include "Core/Math/Quat.h"

namespace Nightbird::Core
{
	NB_TYPE_BASE_IMPL(Quat)
		NB_FIELD(x, Float)
		NB_FIELD(y, Float)
		NB_FIELD(z, Float)
		NB_FIELD(w, Float)
	NB_TYPE_IMPL_END()
}
