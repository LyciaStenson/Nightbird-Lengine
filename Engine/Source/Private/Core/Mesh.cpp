#include "Core/Mesh.h"

namespace Nightbird::Core
{
	Mesh::Mesh(std::vector<MeshPrimitive> primitives)
		: m_Primitives(std::move(primitives))
	{

	}

	const std::vector<MeshPrimitive>& Mesh::GetPrimitives() const
	{
		return m_Primitives;
	}

	size_t Mesh::GetPrimitiveCount() const
	{
		return m_Primitives.size();
	}
}
