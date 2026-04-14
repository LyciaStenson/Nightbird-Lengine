#pragma once

#include "Core/TypeInfo.h"

#include <vector>
#include <string_view>

namespace Nightbird
{
	class TypeRegistry
	{
	public:
		static void Register(TypeInfo* type) noexcept;

		static const TypeInfo* Find(std::string_view name) noexcept;
		static const TypeInfo* Find(uint32_t hash) noexcept;

		static const std::vector<TypeInfo*>& GetAll() noexcept;

	private:
		static std::vector<TypeInfo*> s_Types;
	};
}