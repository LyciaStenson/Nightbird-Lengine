#include "Import/StbTextureImporter.h"

#include "Core/Texture.h"
#include "Core/Log.h"

#include <stb_image.h>

#include <cstring>

namespace Nightbird::Editor
{
	bool StbTextureImporter::SupportsExtension(const std::string& extension) const
	{
		return extension == ".jpg" || extension == ".png" || extension == ".tga" || extension == ".bmp" || extension == ".psd" || extension == ".gif" || extension == ".hdr" || extension == ".pic";
	}

	std::shared_ptr<Core::Texture> StbTextureImporter::Load(const AssetInfo& assetInfo)
	{
		return nullptr;
	}
}
