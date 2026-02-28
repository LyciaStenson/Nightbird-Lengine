#pragma once

#include "Core/MeshPrimitive.h"

#include <vector>
#include <memory>

namespace Nightbird::Core
{
	class Mesh
	{
	public:
		Mesh(std::vector<MeshPrimitive> primitives);

		const std::vector<MeshPrimitive>& GetPrimitives() const;
		size_t GetPrimitiveCount() const;

	private:
		std::vector<MeshPrimitive> m_Primitives;
	};
}