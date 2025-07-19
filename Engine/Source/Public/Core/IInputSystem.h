#pragma once

//#include <functional>
//#include <string>
//
//class IInputSystem
//{
//public:
//	using ActionCallback = std::function<void(const std::string&)>;
//
//	virtual ~IInputSystem() = default;
//
//	virtual void BindKey(const std::string& action, int key) = 0;
//	virtual void BindMouseButton(const std::string& action, int button) = 0;
//
//	virtual void SubscribeActionPressed(const std::string& action, ActionCallback callback) = 0;
//	virtual void SubscribeActionReleased(const std::string& action, ActionCallback callback) = 0;
//};