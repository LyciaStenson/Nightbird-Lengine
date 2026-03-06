#pragma once

#include <uuid.h>

#include <memory>
#include <filesystem>

namespace Nightbird::Core
{
	class Scene;
}

namespace Nightbird::Editor
{
	class ImportManager;

	struct SceneReadResult
	{
		std::unique_ptr<Core::Scene> scene;
		uuids::uuid uuid;
	};

	class TextSceneReader
	{
	public:
		TextSceneReader(ImportManager& importManager);
		SceneReadResult Read(const std::filesystem::path& path);

	private:
		ImportManager& m_ImportManager;
	};
}