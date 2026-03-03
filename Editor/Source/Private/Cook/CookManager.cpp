#include "Cook/CookManager.h"

#include "Core/SceneObject.h"
#include "Core/SceneInstance.h"
#include "Core/MeshInstance.h"
#include "Core/Texture.h"
#include "Core/Material.h"
#include "Core/Mesh.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	CookManager::CookManager(const std::filesystem::path& outputDir)
		: m_OutputDir(outputDir)
	{

	}

	void CookManager::Cook(Core::SceneObject* root, CookTarget target)
	{
		m_TextureUUIDs.clear();
		m_MaterialUUIDs.clear();
		m_MeshUUIDs.clear();

		Endianness endianness = GetEndianness(target);
		std::filesystem::path outputDir = GetOutputDir(target);

		uuids::uuid sceneUUID;
		std::string sceneName = root->GetName();
		Core::SceneObject* sceneRoot = root;

		if (auto* sceneInstance = dynamic_cast<Core::SceneInstance*>(root))
		{
			sceneUUID = sceneInstance->GetSceneUUID();
			sceneRoot = sceneInstance;
		}
		else
		{
			sceneUUID = GenerateUUID();
		}

		CollectAssets(root);

		Core::Log::Info("Cooking " + std::to_string(m_TextureUUIDs.size()) + " textures, "
			+ std::to_string(m_MaterialUUIDs.size()) + " materials"
			+ std::to_string(m_MeshUUIDs.size()) + " meshes");
		
		CookTextures(outputDir, endianness);
		CookMaterials(outputDir, endianness);
		CookMeshes(outputDir, endianness);

		//std::filesystem::path ntScenePath = outputDir / (uuids::to_string(sceneUUID) + ".ntscene");
		//m_TextSceneWriter.Write(sceneRoot, sceneName, sceneUUID, ntScenePath, m_MeshUUIDs);
	}

	void CookManager::CollectAssets(Core::SceneObject* object)
	{
		if (!object)
			return;

		if (auto* meshInstance = dynamic_cast<Core::MeshInstance*>(object))
		{
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

	void CookManager::CookTextures(const std::filesystem::path& outputDir, Endianness endianness)
	{
		for (const auto& [texture, uuid] : m_TextureUUIDs)
			m_TextureCooker.Cook(*texture, uuid, outputDir, endianness);
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
		default:
			return Endianness::Little;
		}
	}

	std::filesystem::path CookManager::GetOutputDir(CookTarget target) const
	{
		switch (target)
		{
		case CookTarget::Desktop:
			return m_OutputDir / "Desktop";
		case CookTarget::WiiU:
			return m_OutputDir / "WiiU";
		default:
			return m_OutputDir / "Desktop";
		}
	}
}