#pragma once

#include <uuid.h>

#include <memory>

namespace Nightbird::Core
{
	class Mesh;
	class AudioAsset;

	class IAssetLoader
	{
	public:
		virtual ~IAssetLoader() = default;
		virtual std::shared_ptr<Mesh> LoadMesh(const uuids::uuid& uuid) = 0;
		virtual std::shared_ptr<AudioAsset> LoadAudio(const uuids::uuid& uuid) = 0;
	};
}