#pragma once

#include "Core/TypeInfo.h"

namespace Nightbird::Detail
{
	template<typename T, typename = void>
	struct HasTypeInfo : std::false_type {};

	template<typename T>
	struct HasTypeInfo<T, std::void_t<decltype(T::s_TypeInfo)>> : std::true_type {};

	template<typename T>
	const TypeInfo* GetFieldTypeInfo()
	{
		if constexpr (HasTypeInfo<T>::value)
			return &T::s_TypeInfo;
		else
			return nullptr;
	}
}
