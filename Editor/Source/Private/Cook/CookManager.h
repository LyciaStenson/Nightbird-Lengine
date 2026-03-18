#pragma once

#include "Cook/Target.h"
#include "Cook/TextureCooker.h"
#include "Cook/MaterialCooker.h"
#include "Cook/MeshCooker.h"

#include "Scene/TextSceneWriter.h"

#include <uuid.h>

#include <filesystem>
#include <unordered_map>
#include <unordered_set>

namespace Nightbird::Core
{
	class SceneObject;
	class Camera;
	class Texture;
	struct Material;
	class Mesh;
}

namespace Nightbird::Editor
{
	class ImportManager;

	//struct ManifestEntry
	//{
		//uuids::uuid uuid;
		//std::string type;
		//std::filesystem::path filePath;
	//};

	class CookManager
	{
	public:
		CookManager(const std::filesystem::path& outputDir, ImportManager& importManager);

		void Cook(const std::filesystem::path& textScenePath, CookTarget target);

	private:
		std::filesystem::path m_RootOutputDir;
		std::filesystem::path m_CookOutputDir;

		Endianness m_Endianness = Endianness::Little;

		ImportManager& m_ImportManager;
		
		TextureCooker m_TextureCooker;
		MaterialCooker m_MaterialCooker;
		MeshCooker m_MeshCooker;

		std::unordered_map<const Core::Texture*, uuids::uuid> m_TextureUUIDs;
		std::unordered_map<const Core::Material*, uuids::uuid> m_MaterialUUIDs;
		std::unordered_map<const Core::Mesh*, uuids::uuid> m_MeshUUIDs;
		
		std::unordered_map<uuids::uuid, std::unique_ptr<Core::SceneObject>> m_ImportedSceneRoots;

		std::unordered_set<uuids::uuid> m_CookedSceneUUIDs;

		//std::vector<ManifestEntry> m_Manifest;

		void WriteBinaryScene(Core::SceneObject* scene, const uuids::uuid& sceneUUID,
			const std::filesystem::path& outputDir, Endianness endianness, Core::Camera* activeCamera = nullptr);
		
		void CollectAssets(Core::SceneObject* object);

		void CookTextures(const std::filesystem::path& outputDir, CookTarget target, Endianness endianness);
		void CookMaterials(const std::filesystem::path& outputDir, Endianness endianness);
		void CookMeshes(const std::filesystem::path& outputDir, Endianness endianness);

		uuids::uuid GenerateUUID() const;
		Endianness GetEndianness(CookTarget target) const;
		std::filesystem::path GetOutputDir(CookTarget target) const;
	};
}
