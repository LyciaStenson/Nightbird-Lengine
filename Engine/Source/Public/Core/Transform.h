#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Nightbird
{
	struct Transform
	{
		Transform();

		glm::mat4 GetLocalMatrix() const;

		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
	};
}