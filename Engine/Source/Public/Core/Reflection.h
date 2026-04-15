#pragma once

#include "Core/TypeInfo.h"
#include "Core/TypeRegistry.h"
#include "Core/ReflectionInternal.h"

#include <iterator>

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

#define NB_PARENT(T) &T::s_TypeInfo
#define NB_NO_PARENT nullptr
#define NB_FACTORY(T) []() -> void* { return new T(); }
#define NB_NO_FACTORY nullptr

#define NB_REFLECT_TYPE(Type, Parent, Factory, ...) \
	::Nightbird::TypeInfo Type::s_TypeInfo = { \
		#Type, \
		::Nightbird::FNVHash(#Type), \
		Parent, \
		Factory, \
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
		static const ::Nightbird::FieldInfo _nb_fields[] = { __VA_ARGS__ }; \
		static const bool _nb_reg_ = ([]() { \
			_NB_CurrentType::s_TypeInfo.fields = _nb_fields; \
			_NB_CurrentType::s_TypeInfo.fieldCount = \
				static_cast<uint16_t>(std::size(_nb_fields)); \
			_NB_CurrentType::_nb_RegisterType(); \
			return true; \
		}()); \
	}

#define NB_FIELD(Name, Kind) \
	{ \
		#Name, \
		::Nightbird::FNVHash(#Name), \
		::Nightbird::Detail::GetFieldTypeInfo<std::decay_t<decltype(_NB_CurrentType::Name)>>(), \
		::Nightbird::FieldKind::Kind \
	}
