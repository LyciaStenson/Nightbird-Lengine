#pragma once

#include <filesystem>

namespace Nightbird::Editor
{
	struct ProjectConfig
	{
		std::string name;
	};

	ProjectConfig LoadProjectConfig(const std::filesystem::path& projectPath);

	void GeneratePremake(const ProjectConfig& projectConfig, const std::filesystem::path& installPath, const std::filesystem::path& projectPath);
}
