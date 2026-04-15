#pragma once

#include "Core/TypeInfo.h"
#include "Core/TypeRegistry.h"
#include "Core/ReflectionInternal.h"

#define NB_CONCAT_INNER(a, b) a##b
#define NB_CONCAT(a, b) NB_CONCAT_INNER(a, b)

#define NB_TYPE_BASE() \
	public: \
		static ::Nightbird::TypeInfo s_TypeInfo; \
		virtual const ::Nightbird::TypeInfo* GetTypeInfo() const { return &s_TypeInfo; } \
		static void _nb_RegisterType();

#define NB_TYPE() \
	public: \
		static ::Nightbird::TypeInfo s_TypeInfo; \
		const ::Nightbird::TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; } \
		static void _nb_RegisterType();

#define NB_TYPE_BASE_IMPL(Type) \
	::Nightbird::TypeInfo Type::s_TypeInfo = { \
		#Type, \
		::Nightbird::FNVHash(#Type), \
		nullptr, \
		[]() -> void* { return new Type(); }, \
		nullptr, \
		0 \
	}; \
	void Type::_nb_RegisterType() \
	{ \
		::Nightbird::TypeRegistry::Register(&Type::s_TypeInfo); \
	} \
	namespace NB_Reflection_##Type \
	{ \
		using _NB_CurrentType = Type; \
		static const ::Nightbird::FieldInfo _nb_fields[] = {

#define NB_TYPE_IMPL(Type, Parent) \
	::Nightbird::TypeInfo Type::s_TypeInfo = { \
		#Type, \
		::Nightbird::FNVHash(#Type), \
		&Parent::s_TypeInfo, \
		[]() -> void* { return new Type(); }, \
		nullptr, \
		0 \
	}; \
	void Type::_nb_RegisterType() \
	{ \
		::Nightbird::TypeRegistry::Register(&Type::s_TypeInfo); \
	} \
	namespace NB_Reflection_##Type \
	{ \
		using _NB_CurrentType = Type; \
		static const ::Nightbird::FieldInfo _nb_fields[] = {

#define NB_TYPE_IMPL_END() \
		}; \
		static bool NB_CONCAT(_nb_reg_, __COUNTER__) = ([]() { \
			_NB_CurrentType::s_TypeInfo.fields = _nb_fields; \
			_NB_CurrentType::s_TypeInfo.fieldCount = \
				static_cast<uint16_t>(sizeof(_nb_fields) / sizeof(_nb_fields[0])); \
			_NB_CurrentType::_nb_RegisterType(); \
			return true; \
		}()); \
	}

#define NB_FIELD(Name, Kind) \
	{ \
		#Name, \
		::Nightbird::FNVHash(#Name), \
		::Nightbird::Detail::GetFieldTypeInfo<std::decay_t<decltype(_NB_CurrentType::Name)>>(), \
		::Nightbird::FieldKind::Kind, \
		[](void* instance) -> void* { \
			return &static_cast<_NB_CurrentType*>(instance)->Name; \
		}, \
		[](void* instance, const void* value) { \
			auto* obj = static_cast<_NB_CurrentType*>(instance); \
			obj->Name = *static_cast<const std::decay_t<decltype(obj->Name)>*>(value); \
		} \
	},

#define NB_REFLECTION_FRIEND \
	friend struct ::Nightbird::ReflectionAccessor;
