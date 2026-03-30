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
			rttr::registration::class_<Type>(#Type) \

	#define NB_CLASS_END() \
		;}

	#define NB_PROPERTY(Name) \
		.property(#Name, &_NB_CurrentType::Name)

	#define NB_CONSTRUCTOR(...) \
		.constructor<__VA_ARGS__>()

	#define NB_ENABLE(...) \
		RTTR_ENABLE(__VA_ARGS__) \
		public:

	#define NB_REGISTRATION_FRIEND \
		RTTR_REGISTRATION_FRIEND

#else
	#define NB_REGISTRATION
	#define NB_REGISTRATION_END
	#define NB_CLASS(Type)
	#define NB_CLASS_END()
	#define NB_PROPERTY(Name)
	#define NB_CONSTRUCTOR(...)
	#define NB_ENABLE(...)
	#define NB_REGISTRATION_FRIEND
#endif
