#pragma once

#include <string>

#include <Core/SceneObject.h>
#include <Core/Transform.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace Nightbird
{
	class VulkanDevice;
	class VulkanUniformBuffer;
	class Mesh;

	class PrefabInstance : public SceneObject
	{
	public:
		PrefabInstance() = default;
		PrefabInstance(const std::string& name, const std::string& prefabPath);
		~PrefabInstance();
		
		const std::string& GetPrefabPath() const;

		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				CEREAL_NVP(name),
				CEREAL_NVP(transform),
				CEREAL_NVP(prefabPath)
			);
		}

	protected:
		std::string prefabPath;
	};
}

CEREAL_REGISTER_TYPE(Nightbird::PrefabInstance)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Nightbird::SceneObject, Nightbird::PrefabInstance)