#include "Import/TextCubemapWriter.h"

#include "Core/Cubemap.h"
#include "Core/Log.h"

#include <toml.hpp>

namespace Nightbird::Editor
{
	void TextCubemapWriter::Write(const Core::Cubemap& cubemap, const uuids::uuid& cubemapUUID, const std::filesystem::path& outputPath)
	{
		toml::table document;

		toml::table cubemapTable;
		cubemapTable.insert("uuid", uuids::to_string(cubemapUUID));
		document.insert("cubemap", cubemapTable);

		const auto& faceUUIDs = cubemap.m_FaceUUIDs;

		toml::table facesTable;
		facesTable.insert("pos_x", uuids::to_string(faceUUIDs[0]));
		facesTable.insert("neg_x", uuids::to_string(faceUUIDs[1]));
		facesTable.insert("pos_y", uuids::to_string(faceUUIDs[2]));
		facesTable.insert("neg_y", uuids::to_string(faceUUIDs[3]));
		facesTable.insert("pos_z", uuids::to_string(faceUUIDs[4]));
		facesTable.insert("neg_z", uuids::to_string(faceUUIDs[5]));
		document.insert("faces", facesTable);

		std::filesystem::create_directories(outputPath.parent_path());
		std::ofstream file(outputPath);
		file << document;
		file.close();

		Core::Log::Info("Written .ntcubemap: " + outputPath.string());
	}
}