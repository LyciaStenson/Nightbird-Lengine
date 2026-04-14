#pragma once

namespace Nightbird::Detail
{
	template<typename T>
	struct TypeToInfo
	{
		static const TypeInfo* Get()
		{
			return nullptr;
		}
	};
}
