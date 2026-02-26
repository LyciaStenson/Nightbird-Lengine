#pragma once

#include "Core/Vertex.h"
#include "Core/Material.h"

#include <vector>

namespace Nightbird
{
	class MeshPrimitive
	{
	public:
		MeshPrimitive(std::vector<Vertex> vertices, std::vector<uint16_t> indices, Material material);

		const std::vector<Vertex>& GetVertices() const;
		const std::vector<uint16_t>& GetIndices() const;
		const Material& GetMaterial() const;

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint16_t> m_Indices;
		Material m_Material;
	};
}