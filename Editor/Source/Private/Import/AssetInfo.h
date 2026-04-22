#pragma once

#include <uuid.h>
#include <toml.hpp>

#include <string>
#include <filesystem>

namespace Nightbird::Editor
{
	//enum class SourceType
	//{
	//	File,
	//	Container,
	//	Generated
	//};

	//enum class AssetType
	//{
	//	Mesh,
	//	Audio,
	//	Texture,
	//	Material,
	//	Scene
	//};

	struct AssetInfo
	{
		uuids::uuid uuid;
		
		//SourceType sourceType;
		//AssetType assetType;

		std::filesystem::path sourcePath;
		std::string importer;
		
		toml::table params;
	};
}