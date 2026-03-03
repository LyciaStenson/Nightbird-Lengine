#include "Core/SceneInstance.h"

namespace Nightbird::Core
{
	SceneInstance::SceneInstance(const std::string& name, const uuids::uuid& sceneUUID)
		: SpatialObject(name), m_SceneUUID(sceneUUID)
	{

	}

	const uuids::uuid& SceneInstance::GetSceneUUID() const
	{
		return m_SceneUUID;
	}
}