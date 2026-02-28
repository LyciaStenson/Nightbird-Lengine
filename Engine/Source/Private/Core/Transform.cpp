#include "Core/Transform.h"

namespace Nightbird::Core
{
	Transform::Transform()
		: position(0.0f), rotation(), scale(1.0f)
	{

	}

	glm::mat4 Transform::GetLocalMatrix() const
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), position);
		glm::mat4 r = glm::mat4_cast(rotation);
		glm::mat4 s = glm::scale(glm::mat4(1.0f), scale);

		return t * r * s;
	}
}