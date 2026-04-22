#include "Core/MeshInstance.h"

NB_REFLECT(Nightbird::Core::MeshInstance, NB_PARENT(Nightbird::Core::SpatialObject), NB_FACTORY(Nightbird::Core::MeshInstance),
	NB_FIELD(m_Mesh)
)

namespace Nightbird::Core
{
	void MeshInstance::ResolveAssets(AssetManager& assetManager)
	{
		//if (!m_Mesh && !m_MeshUUID.is_nil())
			//m_Mesh = assetManager.Get<Mesh>(m_MeshUUID);
	}

	void MeshInstance::EnterScene()
	{

	}
}
