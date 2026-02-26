#include "Core/MeshInstance.h"

namespace Nightbird
{
	MeshInstance::MeshInstance(const std::string& name, std::shared_ptr<Mesh> mesh)
		: SpatialObject(name), m_Mesh(std::move(mesh))
	{

	}

	std::shared_ptr<const Mesh> MeshInstance::GetMesh() const
	{
		return m_Mesh;
	}
}