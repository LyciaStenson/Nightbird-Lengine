#pragma once

#include <string>
#include <functional>
#include <unordered_map>
#include <deque>
#include <bitset>

#include <GLFW/glfw3.h>

#ifdef INPUT_BUILD
#define INPUT_API __declspec(dllexport)
#else
#define INPUT_API __declspec(dllimport)
#endif

namespace Nightbird
{
	struct Binding
	{
		enum class Type { Key, MouseButton } type;
		int code;
	};

	class INPUT_API Input
	{
	public:
		using ActionCallback = std::function<void(const std::string&)>;
		
		static Input& Get();
		static void Shutdown();

		void Init(GLFWwindow* window);
		
		void BindKey(const std::string& action, int key);
		void BindMouseButton(const std::string& action, int button);
		
		void SubscribeActionPressed(const std::string& action, ActionCallback callback);
		void SubscribeActionReleased(const std::string& action, ActionCallback callback);

		void ProcessEvents();

	private:
		struct InputEvent
		{
			enum class Type { Key, MouseButton, MouseMove } type;
			int code;
			int action;
			double x, y;
		};

		void PushEvent(const InputEvent& event);
		void HandleEvent(const InputEvent& event);

		static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
		static void CursorPosCallback(GLFWwindow* window, double x, double y);

		static Input* instance;
		
		GLFWwindow* m_Window = nullptr;
		
		std::deque<InputEvent> m_EventQueue;
		std::bitset<GLFW_KEY_LAST + 1> m_KeyState;
		std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> m_MouseButtonState;
		double m_MouseX = 0.0, m_MouseY = 0.0;
		
		std::unordered_map<std::string, std::vector<Binding>> m_ActionBindings;

		std::unordered_map<std::string, std::vector<ActionCallback>> m_ActionPressedCallbacks;
		std::unordered_map<std::string, std::vector<ActionCallback>> m_ActionReleasedCallbacks;
	};
}