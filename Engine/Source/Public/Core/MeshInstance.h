#pragma once

#include "Core/SpatialObject.h"
#include "Core/Mesh.h"

#include <memory>

namespace Nightbird::Core
{
	class MeshInstance : public SpatialObject
	{
	public:
		NB_OBJECT(MeshInstance, SpatialObject)
		NB_ENABLE(SpatialObject)
		NB_REGISTRATION_FRIEND

		MeshInstance(const std::string& name);
		MeshInstance(const std::string& name, std::shared_ptr<Mesh> mesh);
		~MeshInstance() override = default;

		std::shared_ptr<const Mesh> GetMesh() const;
		void SetMesh(std::shared_ptr<Mesh> mesh);

	private:
		std::shared_ptr<Mesh> m_Mesh;
	};
}
