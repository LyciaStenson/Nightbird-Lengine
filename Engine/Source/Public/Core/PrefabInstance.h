#pragma once

#include <string>

#include <Core/SceneObject.h>
#include <Core/SceneObjectRegistry.h>
#include <Core/Transform.h>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;

	class PrefabInstance : public SceneObject
	{
	public:
		PrefabInstance(const char* name, const char* prefabPath = "");
		PrefabInstance(const std::string& name, const std::string& prefabPath = "");
		~PrefabInstance() override;
		
		const std::string& GetPrefabPath() const;

	protected:
		std::string prefabPath;

		//RTTR_ENABLE(PrefabInstance)
	};
}