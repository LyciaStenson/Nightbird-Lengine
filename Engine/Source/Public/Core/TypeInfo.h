#pragma once

#include <cstdint>
#include <string_view>

namespace Nightbird
{
	struct TypeInfo;
	struct FieldInfo;

	// FNV-1a hash
	constexpr uint32_t FNVHash(std::string_view s) noexcept
	{
		uint32_t h = 2166136261u;
		for (char c : s)
			h = (h ^ static_cast<uint8_t>(c)) * 16777619u;
		return h;
	}

	struct ReflectionAccessor {};
	
	struct TypeInfo
	{
		const char* name = nullptr;
		uint32_t nameHash = 0;

		const TypeInfo* parent = nullptr;

		using FactoryFn = void* (*)();
		FactoryFn factory = nullptr;

		const FieldInfo* fields = nullptr;
		uint16_t fieldCount = 0;

		bool IsA(const TypeInfo* other) const noexcept
		{
			const TypeInfo* current = this;
			while (current)
			{
				if (current == other)
					return true;
				current = current->parent;
			}
			return false;
		}

		bool HasFactory() const noexcept
		{
			return factory != nullptr;
		}

		void* Create() const noexcept
		{
			return factory ? factory() : nullptr;
		}
	};

	enum class FieldKind : uint8_t
	{
		Object, // Has TypeInfo
		Bool, Int32, UInt32, Float, Double, String,
		Vector2, Vector3, Vector4, Quat,
		UUID,
		Unknown
	};

	struct FieldInfo
	{
		const char* name = nullptr;
		uint32_t nameHash = 0;

		const TypeInfo* type = nullptr;
		FieldKind kind = FieldKind::Unknown;
		
		using GetFn = void* (*)(void* instance);
		using SetFn = void (*)(void* instance, const void* value);

		GetFn Get = nullptr;
		SetFn Set = nullptr;
	};
	
	const TypeInfo* Find(std::string_view name) noexcept;
	const TypeInfo* Find(uint32_t hash) noexcept;
	
	template<typename T, typename U>
	T* Cast(U* obj)
	{
		if (obj && obj->GetTypeInfo()->IsA(&T::s_TypeInfo))
			return static_cast<T*>(obj);
		return nullptr;
	}

	template<typename T, typename U>
	const T* Cast(const U* obj)
	{
		if (obj && obj->GetTypeInfo()->IsA(&T::s_TypeInfo))
			return static_cast<const T*>(obj);
		return nullptr;
	}

	template<typename T, typename U>
	bool IsA(const U* obj)
	{
		return obj && obj->GetTypeInfo()->IsA(&T::s_TypeInfo);
	}
}
