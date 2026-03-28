#pragma once

namespace Nightbird
{
	struct TypeInfo
	{
		const char* name;
		const TypeInfo* parent;

		bool IsA(const TypeInfo* other) const
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

#define NB_OBJECT(ClassName, ParentClass) \
	static const ::Nightbird::TypeInfo s_TypeInfo; \
	const ::Nightbird::TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; }

#define NB_OBJECT_BASE(ClassName) \
	static const ::Nightbird::TypeInfo s_TypeInfo; \
	virtual const ::Nightbird::TypeInfo* GetTypeInfo() const { return &s_TypeInfo; }

#define NB_OBJECT_IMPL(ClassName, ParentClass) \
	const ::Nightbird::TypeInfo ClassName::s_TypeInfo = { #ClassName, &ParentClass::s_TypeInfo };

#define NB_OBJECT_BASE_IMPL(ClassName) \
	const ::Nightbird::TypeInfo ClassName::s_TypeInfo = { #ClassName, nullptr };
