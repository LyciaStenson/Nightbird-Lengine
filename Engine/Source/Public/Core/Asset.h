#pragma once

#include "Core/TypeInfo.h"

namespace Nightbird::Core
{
	class Asset
	{
		NB_OBJECT_BASE(Asset)
		virtual ~Asset() = default;
	};
}