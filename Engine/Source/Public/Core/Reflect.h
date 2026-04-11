#pragma once

#ifdef EDITOR_BUILD
	#include <rttr/type>
	#include <rttr/registration>
	#include <rttr/registration_friend>

	#include "Core/Log.h"

	#define NB_REGISTRATION \
		RTTR_REGISTRATION {

	#define NB_REGISTRATION_END \
		}

	#define NB_CLASS(Type) \
		{ \
			using _NB_CurrentType = Type; \
			rttr::registration::class_<Type>(#Type)

	#define NB_CLASS_END() \
		;}

	#define NB_PROPERTY(Name) \
		.property(#Name, &_NB_CurrentType::Name)

	#define NB_CONSTRUCTOR(...) \
		.constructor<__VA_ARGS__>() (rttr::policy::ctor::as_raw_ptr)

	#define NB_ENABLE(...) \
		RTTR_ENABLE(__VA_ARGS__) \
		public:

	#define NB_REGISTRATION_FRIEND \
		RTTR_REGISTRATION_FRIEND

#else
	#include "Core/TypeInfo.h"

	#define NB_CONCAT_INNER(a, b) \
		a##b
	#define NB_CONCAT(a, b) \
		NB_CONCAT_INNER(a, b)

	#define NB_REGISTRATION \
		namespace { static bool NB_CONCAT(_nb_reg_, __COUNTER__) = ([]() -> bool {

	#define NB_REGISTRATION_END \
		return true; }(), true); }

	#define NB_CLASS(Type) \
		{ \
			using _NB_CurrentType = Type; \
			static const ::Nightbird::PropDesc _nb_props[] = {

	#define NB_CLASS_END() \
		}; \
		_NB_CurrentType::s_TypeInfo.props = _nb_props; \
		_NB_CurrentType::s_TypeInfo.propCount = \
			static_cast<uint16_t>(sizeof(_nb_props) / sizeof(_nb_props[0])); \
		_NB_CurrentType::s_TypeInfo.factory = []() -> ::Nightbird::OwnedObject { \
			_NB_CurrentType* p = new _NB_CurrentType(); \
			return { p, [](void* ptr) { \
				delete static_cast<_NB_CurrentType*>(ptr); } }; \
		}; \
		}

	#define NB_PROPERTY(Name) \
		{ \
			#Name, \
			::Nightbird::FNVHash(#Name), \
			static_cast<uint32_t>(offsetof(_NB_CurrentType, Name)), \
			static_cast<uint32_t>(sizeof(_NB_CurrentType::Name)) \
		},

	#define NB_CONSTRUCTOR(...)

	#define NB_ENABLE(...) \
		public:

	#define NB_REGISTRATION_FRIEND
#endif
