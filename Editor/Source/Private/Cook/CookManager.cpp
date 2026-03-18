#include "Cook/CookManager.h"

#include "Core/Scene.h"
#include "Core/SceneObject.h"
#include "Core/MeshInstance.h"
#include "Core/Texture.h"
#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/Log.h"

#include "Import/ImportManager.h"
#include "Import/AssetInfo.h"
#include "Scene/TextSceneReader.h"
#include "Scene/BinarySceneWriter.h"

namespace Nightbird::Editor
{
	CookManager::CookManager(const std::filesystem::path& outputDir, ImportManager& importManager)
		: m_RootOutputDir(outputDir), m_ImportManager(importManager)
	{

	}

	void CookManager::Cook(const std::filesystem::path& textScenePath, CookTarget target)
	{
		m_TextureUUIDs.clear();
		m_MaterialUUIDs.clear();
		m_MeshUUIDs.clear();
		m_ImportedSceneRoots.clear();
		m_CookedSceneUUIDs.clear();
		//m_Manifest.clear();
		
		TextSceneReader sceneReader(m_ImportManager);
		SceneReadResult sceneReadResult = sceneReader.Read(textScenePath);
		if (!sceneReadResult.scene)
		{
			Core::Log::Error("CookManager: Failed to read text scene: " + textScenePath.string());
			return;
		}

		m_Endianness = GetEndianness(target);
		m_CookOutputDir = GetOutputDir(target);
		std::filesystem::create_directories(m_CookOutputDir);
		
		CollectAssets(sceneReadResult.scene->GetRoot());
		
		CookTextures(m_CookOutputDir, target, m_Endianness);
		CookMaterials(m_CookOutputDir, m_Endianness);
		CookMeshes(m_CookOutputDir, m_Endianness);

		for (auto& [uuid, importedRoot] : m_ImportedSceneRoots)
			WriteBinaryScene(importedRoot.get(), uuid, m_CookOutputDir, m_Endianness);

		WriteBinaryScene(sceneReadResult.scene->GetRoot(), sceneReadResult.uuid, m_CookOutputDir, m_Endianness, sceneReadResult.scene->GetActiveCamera());
	}

	void CookManager::WriteBinaryScene(Core::SceneObject* root, const uuids::uuid& sceneUUID,
		const std::filesystem::path& outputDir, Endianness endianness, Core::Camera* activeCamera)
	{
		BinarySceneWriter sceneWriter;
		auto outputPath = outputDir / (uuids::to_string(sceneUUID) + ".nbscene");
		sceneWriter.Write(root, sceneUUID, m_MeshUUIDs, outputPath, endianness, activeCamera);
		//m_Manifest.push_back({ sceneUUID, "scene", outputPath });
		Core::Log::Info("CookManager: Written binary scene: " + outputPath.string());
	}

	void CookManager::CollectAssets(Core::SceneObject* object)
	{
		if (!object)
			return;

		Core::Log::Info("CookManager: CollectAssets visiting: " + object->GetName());
		
		if (object->HasSourceScene())
		{
			if (m_CookedSceneUUIDs.count(object->GetSourceSceneUUID().value()))
				return;
			m_CookedSceneUUIDs.insert(object->GetSourceSceneUUID().value());

			const AssetInfo* assetInfo = m_ImportManager.GetAssetInfo(object->GetSourceSceneUUID().value());
			if (!assetInfo)
			{
				Core::Log::Warning("CookManager: Asset info not found for scene instance: " + object->GetName());
				return;
			}

			auto importedRoot = m_ImportManager.Import(assetInfo->sourcePath);

			for (const auto& child : importedRoot->GetChildren())
				CollectAssets(child.get());

			m_ImportedSceneRoots[object->GetSourceSceneUUID().value()] = std::move(importedRoot);
			return;
		}

		if (auto* meshInstance = object->Cast<Core::MeshInstance>())
		{
			Core::Log::Info("CookManager: Found MeshInstance: " + object->GetName());

			const auto& mesh = meshInstance->GetMesh();
			if (mesh && m_MeshUUIDs.find(mesh.get()) == m_MeshUUIDs.end())
			{
				m_MeshUUIDs[mesh.get()] = GenerateUUID();

				for (size_t i = 0; i < mesh->GetPrimitiveCount(); ++i)
				{
					const Core::MeshPrimitive& primitive = mesh->GetPrimitives()[i];
					const auto& material = primitive.GetMaterial();

					if (material && m_MaterialUUIDs.find(material.get()) == m_MaterialUUIDs.end())
					{
						m_MaterialUUIDs[material.get()] = GenerateUUID();

						if (material->baseColorTexture && m_TextureUUIDs.find(material->baseColorTexture.get()) == m_TextureUUIDs.end())
							m_TextureUUIDs[material->baseColorTexture.get()] = GenerateUUID();

						if (material->metallicRoughnessTexture && m_TextureUUIDs.find(material->metallicRoughnessTexture.get()) == m_TextureUUIDs.end())
							m_TextureUUIDs[material->metallicRoughnessTexture.get()] = GenerateUUID();

						if (material->normalTexture && m_TextureUUIDs.find(material->normalTexture.get()) == m_TextureUUIDs.end())
							m_TextureUUIDs[material->normalTexture.get()] = GenerateUUID();
					}
				}
			}
		}

		for (const auto& child : object->GetChildren())
			CollectAssets(child.get());
	}

	void CookManager::CookTextures(const std::filesystem::path& outputDir, CookTarget target, Endianness endianness)
	{
		for (const auto& [texture, uuid] : m_TextureUUIDs)
			m_TextureCooker.Cook(*texture, uuid, outputDir, target, endianness);
	}

	void CookManager::CookMaterials(const std::filesystem::path& outputDir, Endianness endianness)
	{
		for (const auto& [material, uuid] : m_MaterialUUIDs)
			m_MaterialCooker.Cook(*material, uuid, outputDir, endianness, m_TextureUUIDs);
	}

	void CookManager::CookMeshes(const std::filesystem::path& outputDir, Endianness endianness)
	{
		for (const auto& [mesh, uuid] : m_MeshUUIDs)
			m_MeshCooker.Cook(*mesh, uuid, outputDir, endianness, m_MaterialUUIDs);
	}
	
	uuids::uuid CookManager::GenerateUUID() const
	{
		std::random_device randomDevice;

		auto seedData = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seedData), std::end(seedData), std::ref(randomDevice));
		std::seed_seq seq(std::begin(seedData), std::end(seedData));
		std::mt19937 generator(seq);
		uuids::uuid_random_generator gen{generator};

		return gen();
	}

	Endianness CookManager::GetEndianness(CookTarget target) const
	{
		switch (target)
		{
		case CookTarget::Desktop:
			return Endianness::Little;
		case CookTarget::WiiU:
			return Endianness::Big;
		case CookTarget::N3DS:
			return Endianness::Little;
		default:
			return Endianness::Little;
		}
	}

	std::filesystem::path CookManager::GetOutputDir(CookTarget target) const
	{
		switch (target)
		{
		case CookTarget::Desktop:
			return m_RootOutputDir / "Desktop";
		case CookTarget::WiiU:
			return m_RootOutputDir / "WiiU";
		case CookTarget::N3DS:
			return m_RootOutputDir / "3DS";
		default:
			return m_RootOutputDir / "Desktop";
		}
	}
}
