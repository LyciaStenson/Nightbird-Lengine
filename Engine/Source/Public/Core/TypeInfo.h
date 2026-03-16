#pragma once

namespace Nightbird::Core
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
}

#define NB_OBJECT(ClassName, ParentClass) \
	static const ::Nightbird::Core::TypeInfo s_TypeInfo; \
	const ::Nightbird::Core::TypeInfo* GetTypeInfo() const override { return &s_TypeInfo; }

#define NB_OBJECT_BASE(ClassName) \
	static const ::Nightbird::Core::TypeInfo s_TypeInfo; \
	virtual const ::Nightbird::Core::TypeInfo* GetTypeInfo() const { return &s_TypeInfo; }

#define NB_OBJECT_IMPL(ClassName, ParentClass) \
	const ::Nightbird::Core::TypeInfo ClassName::s_TypeInfo = { #ClassName, &ParentClass::s_TypeInfo };

#define NB_OBJECT_BASE_IMPL(ClassName) \
	const ::Nightbird::Core::TypeInfo ClassName::s_TypeInfo = { #ClassName, nullptr };
