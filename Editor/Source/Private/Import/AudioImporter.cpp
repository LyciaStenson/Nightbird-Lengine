#include "Import/AudioImporter.h"

namespace Nightbird::Editor
{
	std::string AudioImporter::GetName() const
	{
		return "audio";
	}

	bool AudioImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".wav" || extension == ".flac";
	}

	std::unique_ptr<Core::SceneObject> AudioImporter::Import(const AssetInfo& assetInfo)
	{
		return nullptr;
	}
}
