#include "Core/Transform.h"

#include "Core/Reflect.h"
#include "Core/Log.h"

namespace Nightbird::Core
{
	NB_OBJECT_BASE_IMPL(Transform)

	glm::mat4 Transform::GetLocalMatrix() const
	{
		glm::mat4 t = glm::translate(glm::mat4(1.0f), glm::vec3(position));
		glm::mat4 r = glm::mat4_cast(glm::quat(rotation));
		glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

		return t * r * s;
	}
}
