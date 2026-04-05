#include "ProjectConfig.h"

#include "Core/Log.h"

#include <toml.hpp>

#include <fstream>
#include <sstream>

namespace Nightbird::Editor
{
	ProjectConfig LoadProjectConfig(const std::filesystem::path& projectPath)
	{
		ProjectConfig config;

		if (!std::filesystem::exists(projectPath))
		{
			Nightbird::Core::Log::Error("Project file does not exist: " + projectPath.string());
			return config;
		}

		auto parse = toml::parse_file(projectPath.string());
		if (!parse)
		{
			Nightbird::Core::Log::Error("Failed to parse .nproject: " + projectPath.string());
			return config;
		}

		toml::table table = parse.table();

		config.name = table["project"]["name"].value_or(std::string{});

		return config;
	}

	void GeneratePremake(const ProjectConfig& projectConfig, const std::filesystem::path& installPath, const std::filesystem::path& projectPath)
	{
		std::filesystem::path templatePath = installPath / "Templates" / "premake5.template.lua";
		std::filesystem::path outputPath = projectPath / "premake5.lua";

		if (!std::filesystem::exists(templatePath))
		{
			Nightbird::Core::Log::Error("Premake template not found at: " + templatePath.string());
			return;
		}

		std::ifstream templateFile(templatePath);
		std::stringstream buffer;
		buffer << templateFile.rdbuf();
		std::string premakeContent = buffer.str();

		size_t pos = 0;
		const std::string placeholder = "%PROJECT_NAME%";
		while ((pos = premakeContent.find(placeholder, pos)) != std::string::npos)
		{
			premakeContent.replace(pos, placeholder.length(), projectConfig.name);
			pos += projectConfig.name.size();
		}

		std::ofstream outFile(outputPath);
		outFile << premakeContent;

		Nightbird::Core::Log::Info("Generated premake5.lua for project: " + projectConfig.name);
	}
}
