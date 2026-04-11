#pragma once

#include <cstdint>
#include <string_view>

namespace Nightbird
{
	// FNV-1a hash
	constexpr uint32_t FNVHash(std::string_view s) noexcept
	{
		uint32_t h = 2166136261u;
		for (char c : s)
			h = (h ^ static_cast<uint8_t>(c)) * 16777619u;
		return h;
	}

	struct PropDesc
	{
		const char* name;
		uint32_t nameHash;
		uint32_t offset;
		uint32_t size;
	};

	struct OwnedObject
	{
		void* ptr = nullptr;
		void (*deleter)(void*) = nullptr;

		OwnedObject() = default;
		OwnedObject(void* p, void(*d)(void*)) noexcept : ptr(p), deleter(d) {}

		OwnedObject(const OwnedObject&) = delete;
		OwnedObject& operator=(const OwnedObject&) = delete;

		OwnedObject(OwnedObject&& other) noexcept
			: ptr(other.ptr), deleter(other.deleter)
		{
			other.ptr = nullptr;
		}

		OwnedObject& operator=(OwnedObject&& other) noexcept
		{
			if (ptr && deleter)
				deleter(ptr);

			ptr = other.ptr;
			deleter = other.deleter;
			other.ptr = nullptr;
			return *this;
		}

		~OwnedObject()
		{
			if (ptr && deleter)
				deleter(ptr);
		}

		bool IsValid() const noexcept
		{
			return ptr != nullptr;
		}

		template<typename T>
		const T* As() const noexcept
		{
			return static_cast<const T*>(ptr);
		}

		template<typename T>
		T* As() noexcept
		{
			return static_cast<const T*>(ptr);
		}

		void* Release() noexcept
		{
			void* p = ptr;
			ptr = nullptr;
			return p;
		}
	};
	
	struct TypeInfo
	{
		const char* name;
		uint32_t nameHash;
		const TypeInfo* parent;

		using FactorFn = OwnedObject(*)();
		FactorFn factory = nullptr;

		const PropDesc* props = nullptr;
		uint16_t propCount = 0;

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

		static void Register(TypeInfo* info) noexcept;
		static const TypeInfo* Find(std::string_view name) noexcept;
		static const TypeInfo* Find(uint32_t nameHash) noexcept;
	};
	
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

#define NB_OBJECT_BASE(ClassName) \
	public: \
	inline static ::Nightbird::TypeInfo s_TypeInfo = { #ClassName, ::Nightbird::FNVHash(#ClassName), nullptr }; \
	virtual const ::Nightbird::TypeInfo* GetTypeInfo() const { return &s_TypeInfo; } \
	static inline bool _nb_registered = (::Nightbird::TypeInfo::Register(&s_TypeInfo), true);

#define NB_OBJECT(ClassName, ParentClass) \
	public: \
	inline static ::Nightbird::TypeInfo s_TypeInfo = { #ClassName, ::Nightbird::FNVHash(#ClassName), &ParentClass::s_TypeInfo }; \
	const ::Nightbird::TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; } \
	static inline bool _nb_registered = (::Nightbird::TypeInfo::Register(&s_TypeInfo), true);
