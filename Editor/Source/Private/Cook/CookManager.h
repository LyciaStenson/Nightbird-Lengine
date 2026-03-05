#pragma once

#include "Cook/TextureCooker.h"
#include "Cook/MaterialCooker.h"
#include "Cook/MeshCooker.h"

#include "Scene/TextSceneWriter.h"

#include <uuid.h>

#include <filesystem>

namespace Nightbird::Core
{
	class SceneObject;
	class Texture;
	struct Material;
	class Mesh;
}

namespace Nightbird::Editor
{
	class ImportManager;

	enum class CookTarget
	{
		Desktop, WiiU
	};

	struct ManifestEntry
	{
		uuids::uuid uuid;
		std::string type;
		std::filesystem::path filePath;
	};

	class CookManager
	{
	public:
		CookManager(const std::filesystem::path& outputDir, ImportManager& importManager);

		void Cook(const std::filesystem::path& textScenePath, CookTarget target);

	private:
		std::filesystem::path m_OutputDir;
		ImportManager& m_ImportManager;
		
		TextureCooker m_TextureCooker;
		MaterialCooker m_MaterialCooker;
		MeshCooker m_MeshCooker;

		std::unordered_map<const Core::Texture*, uuids::uuid> m_TextureUUIDs;
		std::unordered_map<const Core::Material*, uuids::uuid> m_MaterialUUIDs;
		std::unordered_map<const Core::Mesh*, uuids::uuid> m_MeshUUIDs;

		std::vector<ManifestEntry> m_Manifest;
		
		void CollectAssets(Core::SceneObject* object);

		void CookTextures(const std::filesystem::path& outputDir, Endianness endianness);
		void CookMaterials(const std::filesystem::path& outputDir, Endianness endianness);
		void CookMeshes(const std::filesystem::path& outputDir, Endianness endianness);

		uuids::uuid GenerateUUID() const;
		Endianness GetEndianness(CookTarget target) const;
		std::filesystem::path GetOutputDir(CookTarget target) const;
	};
}