#pragma once

#include <rttr/type>
#include <nlohmann/json.hpp>

namespace rttr
{
	void to_json(nlohmann::json& json, const rttr::variant& variant);
	void from_json(const nlohmann::json& in, rttr::variant& variant);
}