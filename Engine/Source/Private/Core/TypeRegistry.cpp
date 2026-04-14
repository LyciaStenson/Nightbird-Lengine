#include "Core/TypeRegistry.h"

namespace Nightbird
{
	std::vector<TypeInfo*> TypeRegistry::s_Types;

	void TypeRegistry::Register(TypeInfo* type) noexcept
	{
		if (!type)
			return;

		for (TypeInfo* existing : s_Types)
		{
			if (existing == type)
				return;

			if (existing->nameHash == type->nameHash)
				return;
		}

		s_Types.push_back(type);
	}

	const TypeInfo* TypeRegistry::Find(std::string_view name) noexcept
	{
		const uint32_t hash = FNVHash(name);
		return Find(hash);
	}

	const TypeInfo* TypeRegistry::Find(uint32_t hash) noexcept
	{
		for (TypeInfo* type : s_Types)
		{
			if (type && type->nameHash == hash)
				return type;
		}

		return nullptr;
	}

	const std::vector<TypeInfo*>& TypeRegistry::GetAll() noexcept
	{
		return s_Types;
	}
}