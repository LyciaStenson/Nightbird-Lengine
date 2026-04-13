#pragma once

#include <string>

namespace Nightbird::Editor
{
	class SceneImporter;
	class AudioImporter;

	class Importer
	{
	public:
		virtual ~Importer() = default;
		virtual std::string GetName() const = 0;
		virtual bool SupportsExtension(const std::string& extensions) const = 0;
		virtual SceneImporter* AsSceneImporter() { return nullptr; }
		virtual AudioImporter* AsAudioImporter() { return nullptr; }
	};
}
