#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <nlohmann/json.hpp>

namespace glm
{
	inline void to_json(nlohmann::json& json, const glm::vec3& vector)
	{
		json = nlohmann::json::array({vector.x, vector.y, vector.z});
	}

	inline void from_json(const nlohmann::json& json, glm::vec3& vector)
	{
		vector = glm::vec3(json.at(0), json.at(1), json.at(2));
	}

	inline void to_json(nlohmann::json& json, const glm::quat& quat)
	{
		json = nlohmann::json::array({quat.w, quat.x, quat.y, quat.z});
	}

	inline void from_json(const nlohmann::json& json, glm::quat& vector)
	{
		vector = glm::quat(json.at(0), json.at(1), json.at(2), json.at(3));
	}
}