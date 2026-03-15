#include "Load/MeshLoader.h"

#include "Load/BinaryReader.h"
#include "Load/MaterialLoader.h"

#include "Core/Mesh.h"
#include "Core/MeshPrimitive.h"
#include "Core/Log.h"

#include <array>
#include <cstdint>

namespace Nightbird::Load
{
	MeshLoader::MeshLoader(MaterialLoader& materialLoader)
		: m_MaterialLoader(materialLoader)
	{

	}

	std::shared_ptr<Core::Mesh> MeshLoader::Load(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		std::string path = cookedDir + "/" + uuids::to_string(uuid) + ".nbmesh";
		
		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("MeshLoader: Failed to open: " + path);
			return nullptr;
		}

		// Validate type		
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'M' || type[1] != 'E' || type[2] != 'S' || type[3] != 'H')
		{
			Core::Log::Error("MeshLoader: Invalid type signature in: " + path);
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("MeshLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		uint32_t primitiveCount = reader.ReadUInt32();

		std::vector<Core::MeshPrimitive> primitives;
		primitives.reserve(primitiveCount);

		for (uint32_t i = 0; i < primitiveCount; i++)
		{
			uint32_t vertexCount = reader.ReadUInt32();
			auto vertices = reader.ReadVertices(vertexCount);

			uint32_t indexCount = reader.ReadUInt32();
			auto indices = reader.ReadIndices(indexCount);

			uint8_t hasMaterial = reader.ReadUInt8();
			std::shared_ptr<Core::Material> material;
			if (hasMaterial)
			{
				std::array<uint8_t, 16> uuidBytes;
				reader.ReadRawBytes(uuidBytes.data(), 16);
				uuids::uuid materialUUID(uuidBytes);
				material = LoadMaterial(cookedDir, materialUUID);
			}

			primitives.emplace_back(std::move(vertices), std::move(indices), material);
		}

		return std::make_shared<Core::Mesh>(std::move(primitives));
	}

	std::shared_ptr<Core::Material> MeshLoader::LoadMaterial(const std::string& cookedDir, const uuids::uuid& uuid)
	{
		auto it = m_MaterialCache.find(uuid);
		if (it != m_MaterialCache.end())
			return it->second;

		auto material = m_MaterialLoader.Load(cookedDir, uuid);
		m_MaterialCache[uuid] = material;
		return material;
	}
}
