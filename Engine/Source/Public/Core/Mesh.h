#pragma once

#include "Core/TypeInfo.h"

#include "Core/MeshPrimitive.h"

#include <vector>
#include <memory>

namespace Nightbird::Core
{
	class Mesh
	{
	public:
		NB_OBJECT_BASE(Mesh)

		Mesh() = default;
		Mesh(std::vector<MeshPrimitive> primitives);

		const std::vector<MeshPrimitive>& GetPrimitives() const;
		size_t GetPrimitiveCount() const;

	private:
		std::vector<MeshPrimitive> m_Primitives;
	};
}
