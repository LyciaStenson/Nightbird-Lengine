#pragma once

#include "Core/SceneReadResult.h"
#include "Import/Importer.h"

#include <memory>

namespace Nightbird::Core
{
	class SceneObject;
}

namespace Nightbird::Editor
{
	struct AssetInfo;

	class SceneImporter : public Importer
	{
	public:
		SceneImporter* AsSceneImporter() override { return this; }
		virtual Core::SceneReadResult Load(const AssetInfo& assetInfo) = 0;
	};
}
