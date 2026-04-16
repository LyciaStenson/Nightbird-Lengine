#include "Core/TypeInfo.h"

namespace Nightbird
{
	FieldInfo* TypeInfo::Begin() const noexcept
	{
		return fields;
	}

	FieldInfo* TypeInfo::End() const noexcept
	{
		return fields + fieldCount;
	}
}