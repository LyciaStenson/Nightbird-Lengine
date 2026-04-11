#include "Core/MeshInstance.h"

namespace Nightbird::Core
{
	NB_OBJECT_IMPL(MeshInstance, SpatialObject)

	MeshInstance::MeshInstance()
		: MeshInstance(nullptr)
	{

	}

	MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh)
		: SpatialObject(), m_Mesh(std::move(mesh))
	{

	}

	std::shared_ptr<const Mesh> MeshInstance::GetMesh() const
	{
		return m_Mesh;
	}

	void MeshInstance::SetMesh(std::shared_ptr<Mesh> mesh)
	{
		m_Mesh = mesh;
	}
}
