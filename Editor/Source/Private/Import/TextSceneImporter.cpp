#include "Import/TextSceneImporter.h"

namespace Nightbird::Editor
{
	bool TextSceneImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".ntscene";
	}

	std::unique_ptr<Core::SceneObject> TextSceneImporter::Load(const AssetInfo& assetInfo)
	{
		return nullptr;
	}
}
