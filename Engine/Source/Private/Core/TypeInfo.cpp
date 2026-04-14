#include "Core/TypeInfo.h"

#include <Core/Log.h>

#include <cassert>
#include <unordered_map>

namespace Nightbird
{
	static std::unordered_map<uint32_t, TypeInfo*>& Registry()
	{
		static std::unordered_map<uint32_t, TypeInfo*> s_Map;
		return s_Map;
	}

	void TypeInfo::Register(TypeInfo* info) noexcept
	{
		auto [it, inserted] = Registry().emplace(info->nameHash, info);
		if (!inserted)
		{
			if (it->second != info)
			{
				Core::Log::Error("TypeInfo: Register collision: New: " + std::string(info->name) + ", hash: " + std::to_string(info->nameHash) + ", Existing: " + std::string(it->second->name) + ", hash: " + std::to_string(it->second->nameHash));
			}
			assert(it->second == info && "TypeInfo::Register: Hash collision or duplicate registration");
		}
		(void)it;
	}

	const TypeInfo* TypeInfo::Find(std::string_view name) noexcept
	{
		uint32_t hash = FNVHash(name);
		Core::Log::Info("TypeInfo::Find: looking for " + std::string(name) + " using hash " + std::to_string(hash));
		return Find(hash);
	}

	const TypeInfo* TypeInfo::Find(uint32_t hash) noexcept
	{
		auto& r = Registry();
		auto it = r.find(hash);
		return it != r.end() ? it->second : nullptr;
	}
}