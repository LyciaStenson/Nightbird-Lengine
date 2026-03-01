#include "Import/ImportManager.h"

#include "Import/GltfImporter.h"

#include "Core/Log.h"

namespace Nightbird::Editor
{
	ImportManager::ImportManager(const std::filesystem::path& assetsDir)
		: m_AssetsDir(assetsDir)
	{
		m_Importers.push_back(std::make_unique<GltfImporter>());
	}
}