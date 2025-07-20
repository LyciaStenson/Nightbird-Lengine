#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <deque>
#include <bitset>

#include <GLFW/glfw3.h>

#ifdef INPUT_BUILD
	#define INPUT_API __declspec(dllexport)
#else
	#define INPUT_API __declspec(dllimport)
#endif

extern "C"
{
	INPUT_API void Input_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	INPUT_API void Input_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	INPUT_API void Input_CursorPosCallback(GLFWwindow* window, double x, double y);
}

namespace Nightbird
{
	struct InputEvent
	{
		enum class Type { Key, MouseButton, MouseMove } type;
		int code;
		int action;
		double x, y;
	};

	class INPUT_API Input
	{
	public:
		using ActionCallback = std::function<void(const char*)>;
		
		static Input& Get();
		
		void Init(GLFWwindow* window);

		void PrintActions() const;
		
		void BindKey(const std::string& action, int key);
		void BindMouseButton(const std::string& action, int button);
		
		void SubscribeActionPressed(const std::string& action, ActionCallback callback);
		void SubscribeActionReleased(const std::string& action, ActionCallback callback);

		void ProcessEvents();

		void PushEvent(const InputEvent& event);

	private:
		struct Impl;
		Impl* impl;

		Input();
		~Input();

		Input(const Input&) = delete;
		Input& operator=(const Input&) = delete;
	};
}