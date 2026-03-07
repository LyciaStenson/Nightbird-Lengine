#include "Load/MaterialLoader.h"

#include "Load/BinaryReader.h"
#include "Load/TextureLoader.h"

#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/Log.h"

#include <array>

namespace Nightbird::Load
{
	MaterialLoader::MaterialLoader(TextureLoader& textureLoader)
		: m_TextureLoader(textureLoader)
	{

	}

	std::shared_ptr<Core::Material> MaterialLoader::Load(const std::filesystem::path& cookedDir, const uuids::uuid& uuid)
	{
		auto path = cookedDir / (uuids::to_string(uuid) + ".nbmaterial");

		BinaryReader reader(path);
		if (!reader.IsValid())
		{
			Core::Log::Error("MaterialLoader: Failed to open: " + path.string());
			return nullptr;
		}

		// Validate type		
		uint8_t type[4] = {};
		reader.ReadRawBytes(type, 4);
		if (type[0] != 'M' || type[1] != 'A' || type[2] != 'T' || type[3] != 'L')
		{
			Core::Log::Error("MaterialLoader: Invalid type signature in: " + path.string());
			return nullptr;
		}

		// Check Version
		uint32_t version = reader.ReadUInt32();
		if (version != 1)
		{
			Core::Log::Error("MaterialLoader: Unsupported version: " + std::to_string(version));
			return nullptr;
		}

		auto material = std::make_shared<Core::Material>();

		material->baseColorFactor.r = reader.ReadFloat();
		material->baseColorFactor.g= reader.ReadFloat();
		material->baseColorFactor.b = reader.ReadFloat();
		material->baseColorFactor.a = reader.ReadFloat();
		material->metallicFactor = reader.ReadFloat();
		material->roughnessFactor = reader.ReadFloat();
		material->transparencyEnabled = reader.ReadUInt8();
		material->doubleSided = reader.ReadUInt8();

		uint8_t hasBaseColor = reader.ReadUInt8();
		if (hasBaseColor)
		{
			std::array<uint8_t, 16> uuidBytes;
			reader.ReadRawBytes(uuidBytes.data(), 16);
			uuids::uuid textureUUID(uuidBytes);
			material->baseColorTexture = LoadTexture(cookedDir, textureUUID);
		}

		uint8_t hasMetallicRoughness = reader.ReadUInt8();
		if (hasMetallicRoughness)
		{
			std::array<uint8_t, 16> uuidBytes;
			reader.ReadRawBytes(uuidBytes.data(), 16);
			uuids::uuid textureUUID(uuidBytes);
			material->metallicRoughnessTexture = LoadTexture(cookedDir, textureUUID);
		}

		uint8_t hasNormal = reader.ReadUInt8();
		if (hasNormal)
		{
			std::array<uint8_t, 16> uuidBytes;
			reader.ReadRawBytes(uuidBytes.data(), 16);
			uuids::uuid textureUUID(uuidBytes);
			material->normalTexture = LoadTexture(cookedDir, textureUUID);
		}

		return material;
	}

	std::shared_ptr<Core::Texture> MaterialLoader::LoadTexture(const std::filesystem::path& cookedDir, const uuids::uuid& uuid)
	{
		auto it = m_TextureCache.find(uuid);
		if (it != m_TextureCache.end())
			return it->second;

		auto texture = m_TextureLoader.Load(cookedDir, uuid);
		m_TextureCache[uuid] = texture;
		return texture;
	}
}