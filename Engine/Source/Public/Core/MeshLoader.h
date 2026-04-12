#pragma once

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class Mesh;
	struct Material;

	class MaterialLoader;

	class MeshLoader
	{
	public:
		MeshLoader(MaterialLoader& materialLoader);

		std::shared_ptr<Mesh> Load(const std::string& cookedDir, const uuids::uuid& uuid);

	private:
		MaterialLoader& m_MaterialLoader;
		std::unordered_map<uuids::uuid, std::shared_ptr<Material>> m_MaterialCache;

		std::shared_ptr<Material> LoadMaterial(const std::string& cookedDir, const uuids::uuid& uuid);
	};
}
