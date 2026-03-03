#pragma once

#include <memory>
#include <filesystem>

namespace Nightbird::Core
{
	class Scene;
}

namespace Nightbird::Editor
{
	class ImportManager;

	class TextSceneReader
	{
	public:
		TextSceneReader(ImportManager& importManager);
		std::unique_ptr<Core::Scene> Read(const std::filesystem::path& path);

	private:
		ImportManager& m_ImportManager;
	};
}