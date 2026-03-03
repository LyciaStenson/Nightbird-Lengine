#pragma once

#include "Core/SpatialObject.h"

#include <uuid.h>

#include <string>

namespace Nightbird::Core
{
	class SceneInstance : public SpatialObject
	{
	public:
		SceneInstance(const std::string& name, const uuids::uuid& sceneUUID);

		const uuids::uuid& GetSceneUUID() const;

	private:
		uuids::uuid m_SceneUUID;
	};
}