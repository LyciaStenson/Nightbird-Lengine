#include <GetInputSystem.h>

#include <rttr/type>

#include <iostream>

Nightbird::InputSystem* GetInputSystem()
{
	static Nightbird::InputSystem* instance = nullptr;
	if (instance)
		return instance;

	auto type = rttr::type::get_by_name("InputSystem");
	if (!type.is_valid())
	{
		std::cerr << "GetInputSystem Error: InputSystem type not found" << std::endl;
		return nullptr;
	}

	auto getMethod = type.get_method("Get");
	if (!getMethod.is_valid())
	{
		std::cerr << "GetInputSystem Error: InputSystem::Get method not found" << std::endl;
		return nullptr;
	}

	rttr::variant variant = getMethod.invoke({});
	if (!variant.is_valid())
	{
		std::cerr << "GetInputSystem Error: Failed to invoke InputSystem::Get" << std::endl;
		return nullptr;
	}

	instance = &variant.get_value<Nightbird::InputSystem&>();
	return instance;
}