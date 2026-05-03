#include "Import/TextCubemapReader.h"

#include "Core/Cubemap.h"
#include "Core/Log.h"

#include <toml.hpp>

namespace Nightbird::Editor
{
	static uuids::uuid ParseUUID(std::string_view string)
	{
		auto result = uuids::uuid::from_string(string);
		return result.has_value() ? result.value() : uuids::uuid{};
	}

	CubemapReadResult TextCubemapReader::Read(const std::filesystem::path& path)
	{
		CubemapReadResult result;

		if (!std::filesystem::exists(path))
		{
			Core::Log::Error("TextCubemapReader: File not found: " + path.string());
			return result;
		}

		toml::parse_result tomlResult = toml::parse_file(path.string());
		if (!tomlResult)
		{
			Core::Log::Error("TextCubemapReader: Failed to parse: " + path.string());
			return result;
		}

		toml::table& document = tomlResult.table();
		std::string cubemapUUIDString = document["cubemap"]["uuid"].value_or(std::string{});
		auto cubemapUUID = uuids::uuid::from_string(cubemapUUIDString);
		if (!cubemapUUID)
		{
			Core::Log::Error("TextCubemapReader: Invalid cubemap UUID in: " + path.string());
			return result;
		}

		result.uuid = *cubemapUUID;

		auto facesTable = document["faces"];

		auto cubemap = std::make_shared<Core::Cubemap>();

		auto readFace = [&](int index, std::string_view key) -> bool
			{
				auto string = facesTable[key].value<std::string>();
				if (!string)
				{
					Core::Log::Error("TextCubemapReader: Missing face " + std::string(key) + " in: " + path.string());
					return false;
				}
				cubemap->m_FaceUUIDs[index] = ParseUUID(*string);
				return true;
			};

		if (!readFace(0, "pos_x"))
			return result;
		if (!readFace(1, "neg_x"))
			return result;
		if (!readFace(2, "pos_y"))
			return result;
		if (!readFace(3, "neg_y"))
			return result;
		if (!readFace(4, "pos_z"))
			return result;
		if (!readFace(5, "neg_z"))
			return result;

		result.cubemap = cubemap;

		Core::Log::Info("TextCubemapReader: Read cubemap: " + path.string());
		return result;
	}
}