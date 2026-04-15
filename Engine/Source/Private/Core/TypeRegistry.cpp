#include "Core/TypeRegistry.h"

#include <Core/Log.h>

#include <unordered_map>
#include <cassert>

namespace Nightbird
{
	std::vector<TypeInfo*> TypeRegistry::s_Types;

	static std::unordered_map<uint32_t, TypeInfo*>& GetMap() noexcept
	{
		static std::unordered_map<uint32_t, TypeInfo*> s_Map;
		return s_Map;
	}
	
	void TypeRegistry::Register(TypeInfo* type) noexcept
	{
		if (!type)
			return;

		auto& map = GetMap();
		auto [it, inserted] = map.emplace(type->nameHash, type);

		if (!inserted)
		{
			if (it->second != type)
				Core::Log::Error("TypeRegistry: Hash collision or duplicate name: " + std::string(type->name));
			assert(it->second == type && "TypeRegistry: Hash collision or duplicate name");
			return;
		}
		
		s_Types.push_back(type);
	}

	const TypeInfo* TypeRegistry::Find(std::string_view name) noexcept
	{
		return Find(FNVHash(name));
	}

	const TypeInfo* TypeRegistry::Find(uint32_t hash) noexcept
	{
		auto& map = GetMap();
		auto it = map.find(hash);
		return it != map.end() ? it->second : nullptr;
	}

	const std::vector<TypeInfo*>& TypeRegistry::GetAll() noexcept
	{
		return s_Types;
	}
}