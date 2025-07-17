#include <Core/RTTRSerialization.h>

#include <rttr/variant.h>

#include <iostream>

#include <Core/Transform.h>

namespace rttr
{
	void to_json(nlohmann::json& json, const rttr::variant& variant)
	{
		if (!variant.is_valid())
			return;

		const rttr::type type = variant.get_type();
		if (type.is_arithmetic())
		{
			if (type == rttr::type::get<bool>())
				json = variant.to_bool();
			else if (type == rttr::type::get<int>())
				json = variant.to_int();
			else if (type == rttr::type::get<float>())
				json = variant.to_float();
			else if (type == rttr::type::get<double>())
				json = variant.to_double();
		}
		else if (type == rttr::type::get<std::string>())
			json = variant.to_string();
		else if (type.is_class())
		{
			json["__type"] = type.get_name().to_string();

			for (auto& prop : type.get_properties())
			{
				auto propName = prop.get_name().to_string();
				rttr::variant propValue = prop.get_value(variant);

				if (!propValue.is_valid())
					continue;

				nlohmann::json propJson = propValue;
				json[propName] = propJson;
			}
		}
		else
			std::cerr << "Unsupported type: " << type.get_name() << std::endl;
	}

	void from_json(const nlohmann::json& json, rttr::variant& variant)
	{
		if (json.is_null())
		{
			variant = rttr::variant{};
			return;
		}

		if (!json.contains("__type"))
		{
			std::cerr << "Variant missing __type" << std::endl;
			return;
		}

		std::string typeName = json.at("__type").get<std::string>();
		rttr::type type = rttr::type::get_by_name(typeName);

		if (!type.is_valid())
		{
			std::cerr << "Unknown type: " << typeName << std::endl;
			return;
		}

		rttr::variant object = type.create();
		if (!object)
		{
			std::cerr << "Failed to create instance of type: " << typeName << std::endl;
			return;
		}

		for (auto& property : type.get_properties())
		{
			const auto propertyName = property.get_name().to_string();
			if (json.contains(propertyName))
			{
				const auto& propertyJson = json.at(propertyName);

				rttr::type propertyType = property.get_type();

				rttr::variant propertyValue;

				if (propertyType.is_class() && propertyJson.is_object() && propertyJson.contains("__type"))
					from_json(propertyJson, propertyValue);
				else
				{
					if (propertyType.is_arithmetic())
					{
						if (propertyType == rttr::type::get<bool>())
							propertyValue = propertyJson.get<bool>();
						else if (propertyType == rttr::type::get<int>())
							propertyValue = propertyJson.get<int>();
						else if (propertyType == rttr::type::get<float>())
							propertyValue = propertyJson.get<float>();
						else if (propertyType == rttr::type::get<double>())
							propertyValue = propertyJson.get<double>();
					}
					else if (propertyType == rttr::type::get<std::string>())
						propertyValue = propertyJson.get<std::string>();
				}

				if (propertyValue.is_valid())
					property.set_value(object, propertyValue);
			}
		}

		variant = object;
	}
}