#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Serialization/GlmSerialization.h>

namespace Nightbird
{
	class SceneObject;

	class Transform
	{
	public:
		Transform();
		Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
		~Transform();
		
		glm::mat4 GetLocalMatrix() const;
		
		template <class Archive>
		void serialize(Archive& archive)
		{
			archive
			(
				CEREAL_NVP(position),
				CEREAL_NVP(rotation),
				CEREAL_NVP(scale)
			);
		}

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
}