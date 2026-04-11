#pragma once

#include "Core/TypeInfo.h"

namespace Nightbird::Core
{
	class Asset
	{
	public:
		NB_OBJECT_BASE(Asset)
		virtual ~Asset() = default;
	};
}