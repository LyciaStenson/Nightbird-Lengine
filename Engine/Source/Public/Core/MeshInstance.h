#pragma once

#include "Core/SpatialObject.h"
#include "Core/Mesh.h"

#include <memory>

namespace Nightbird
{
	class MeshInstance : public SpatialObject
	{
	public:
		MeshInstance(const std::string& name, std::shared_ptr<Mesh> mesh);
		~MeshInstance() override = default;

		std::shared_ptr<const Mesh> GetMesh() const;

	private:
		std::shared_ptr<Mesh> m_Mesh;
	};
}