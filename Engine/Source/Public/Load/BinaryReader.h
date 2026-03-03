#pragma once

#include <filesystem>
#include <fstream>

namespace Nightbird::Core
{
	struct Vertex;
}

namespace Nightbird::Load
{
	class BinaryReader
	{
	public:
		BinaryReader(const std::filesystem::path& path);
		bool IsValid() const;

		uint8_t ReadUInt8();
		uint16_t ReadUInt16();
		uint32_t ReadUInt32();
		float ReadFloat();
		Core::Vertex ReadVertex();
		std::vector<Core::Vertex> ReadVertices(uint32_t count);
		std::vector<uint16_t> ReadIndices(uint32_t count);
		void ReadRawBytes(uint8_t* destination, size_t size);

	private:
		std::ifstream m_File;

		template<typename T>
		T ReadValue();
	};
}