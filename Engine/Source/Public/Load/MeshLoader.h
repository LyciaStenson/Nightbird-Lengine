#pragma once

#include <uuid.h>

#include <memory>
#include <filesystem>

namespace Nightbird::Core
{
	class Mesh;
	struct Material;
}

namespace Nightbird::Load
{
	class MaterialLoader;

	class MeshLoader
	{
	public:
		MeshLoader(MaterialLoader& materialLoader);

		std::shared_ptr<Core::Mesh> Load(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);

	private:
		MaterialLoader& m_MaterialLoader;
		std::unordered_map<uuids::uuid, std::shared_ptr<Core::Material>> m_MaterialCache;

		std::shared_ptr<Core::Material> LoadMaterial(const std::filesystem::path& cookedDir, const uuids::uuid& uuid);
	};
}