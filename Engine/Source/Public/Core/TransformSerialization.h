#pragma once

#include <Core/Transform.h>

#include <nlohmann/json.hpp>
#include <Core/GlmSerialization.h>

namespace Nightbird
{
	inline void to_json(nlohmann::json& json, const Transform& transform)
	{
		json = nlohmann::json
		{
			{"position", transform.position},
			{"rotation", transform.rotation},
			{"scale", transform.scale}
		};
	}

	inline void from_json(const nlohmann::json& json, Transform& transform)
	{
		json.at("position").get_to(transform.position);
		json.at("rotation").get_to(transform.rotation);
		json.at("scale").get_to(transform.scale);
	}
}