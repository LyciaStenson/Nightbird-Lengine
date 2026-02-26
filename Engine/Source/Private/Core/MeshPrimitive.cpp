#include "Core/MeshPrimitive.h"

namespace Nightbird
{
	MeshPrimitive::MeshPrimitive(std::vector<Vertex> vertices, std::vector<uint16_t> indices, Material material)
		: m_Vertices(std::move(vertices)), m_Indices(std::move(indices)), m_Material(std::move(material))
	{

	}

	const std::vector<Vertex>& MeshPrimitive::GetVertices() const
	{
		return m_Vertices;
	}

	const std::vector<uint16_t>& MeshPrimitive::GetIndices() const
	{
		return m_Indices;
	}
	
	const Material& MeshPrimitive::GetMaterial() const
	{
		return m_Material;
	}
}