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
