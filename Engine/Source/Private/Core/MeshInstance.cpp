#include "Core/MeshInstance.h"

namespace Nightbird::Core
{
	NB_REFLECT_TYPE(MeshInstance, NB_PARENT(SpatialObject), NB_FACTORY(MeshInstance),
		NB_FIELD(m_MeshUUID, UUID)
	)

	MeshInstance::MeshInstance()
		: SpatialObject(), m_Mesh(std::make_shared<Mesh>())
	{

	}

	MeshInstance::MeshInstance(std::shared_ptr<Mesh> mesh)
		: SpatialObject(), m_Mesh(std::move(mesh))
	{

	}

	void MeshInstance::LoadAssets(AssetLoader& assetLoader)
	{
		m_Mesh = assetLoader.LoadMesh(m_MeshUUID);
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
