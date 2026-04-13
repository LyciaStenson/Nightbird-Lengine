#pragma once

#include "Core/SceneReadResult.h"
#include "Core/TypeInfo.h"

#include <uuid.h>

#include <memory>
#include <string>

namespace Nightbird::Core
{
	class Camera;
	struct Transform;

	class AssetLoader;
	class BinaryReader;

	class BinarySceneReader
	{
	public:
		BinarySceneReader(AssetLoader& assetLoader);

		SceneReadResult Read(const std::string& cookedDir, const uuids::uuid& uuid);

	private:
		AssetLoader& m_AssetLoader;

		void ReadProperties(SceneObject* object, const TypeInfo* typeInfo, uint16_t propCount, BinaryReader& reader);
		void ReadIntoDesc(uint8_t* objectBase, const TypeInfo* typeInfo, uint32_t incomingHash, uint16_t incomingSize, BinaryReader& reader);
	};
}
