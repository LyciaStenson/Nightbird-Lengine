#pragma once

#include "Core/SpatialObject.h"
#include "Core/Mesh.h"

#include <memory>

namespace Nightbird::Core
{
	class MeshInstance : public SpatialObject
	{
	public:
		NB_TYPE()

		MeshInstance();
		MeshInstance(std::shared_ptr<Mesh> mesh);
		~MeshInstance() override = default;

		void LoadAssets(AssetLoader& assetLoader) override;

		std::shared_ptr<const Mesh> GetMesh() const;
		void SetMesh(std::shared_ptr<Mesh> mesh);

		uuids::uuid m_MeshUUID;

	private:
		std::shared_ptr<Mesh> m_Mesh;
	};
}
