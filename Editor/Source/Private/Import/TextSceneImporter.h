#pragma once

#include "Import/SceneImporter.h"

#include "Core/SceneObject.h"

namespace Nightbird::Editor
{
	class TextSceneImporter : public SceneImporter
	{
	public:
		std::string GetName() const override { return "TextScene"; }
		bool SupportsExtension(const std::string& extension) const override;
		std::unique_ptr<Core::SceneObject> Load(const AssetInfo& assetInfo) override;
	};
}
