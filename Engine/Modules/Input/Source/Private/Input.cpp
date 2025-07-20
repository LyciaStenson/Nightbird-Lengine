#include <Input.h>

#include <iostream>

namespace Nightbird
{
	struct Binding
	{
		enum class Type { Key, MouseButton } type;
		int code;
	};
	
	struct Input::Impl
	{
		GLFWwindow* m_Window = nullptr;

		std::deque<InputEvent> m_EventQueue;
		std::bitset<GLFW_KEY_LAST + 1> m_KeyState;
		std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> m_MouseButtonState;
		double m_MouseX = 0.0, m_MouseY = 0.0;

		std::unordered_map<std::string, std::vector<Binding>> m_ActionBindings;

		std::unordered_map<std::string, std::vector<ActionCallback>> m_ActionPressedCallbacks;
		std::unordered_map<std::string, std::vector<ActionCallback>> m_ActionReleasedCallbacks;

		void PushEvent(const InputEvent& event)
		{
			m_EventQueue.push_back(event);
		}

		void HandleEvent(const InputEvent& event)
		{
			switch (event.type)
			{
			case InputEvent::Type::Key:
				if (event.code >= 0 && event.code <= GLFW_KEY_LAST)
					m_KeyState[event.code] = (event.action != GLFW_RELEASE);

				if (event.action == GLFW_PRESS)
				{
					for (const auto& [action, bindings] : m_ActionBindings)
					{
						for (const auto& binding : bindings)
						{
							if (binding.type == Binding::Type::Key && binding.code == event.code)
							{
								auto it = m_ActionPressedCallbacks.find(action);
								if (it != m_ActionPressedCallbacks.end())
								{
									for (auto& callback : it->second)
										if (callback)
											callback(action.c_str());
								}
							}
						}
					}
				}
				else if (event.action == GLFW_RELEASE)
				{
					for (const auto& [action, bindings] : m_ActionBindings)
					{
						for (const auto& binding : bindings)
						{
							if (binding.type == Binding::Type::Key && binding.code == event.code)
							{
								auto it = m_ActionReleasedCallbacks.find(action);
								if (it != m_ActionReleasedCallbacks.end())
								{
									for (auto& callback : it->second)
										if (callback)
											callback(action.c_str());
								}
							}
						}
					}
				}
				break;
			case InputEvent::Type::MouseButton:
				if (event.code >= 0 && event.code <= GLFW_MOUSE_BUTTON_LAST)
					m_MouseButtonState[event.code] = (event.action != GLFW_RELEASE);
				break;
			case InputEvent::Type::MouseMove:
				m_MouseX = event.x;
				m_MouseY = event.y;
				break;
			default:
				break;
			}
		}
	};
	
	Input::Input()
		: impl(new Impl())
	{

	}

	Input::~Input()
	{
		delete impl;
	}

	Input& Input::Get()
	{
		static Input instance;
		return instance;
	}
	
	void Input::Init(GLFWwindow* window)
	{
		impl->m_Window = window;

		glfwSetKeyCallback(window, Input_KeyCallback);
		glfwSetMouseButtonCallback(window, Input_MouseButtonCallback);
		glfwSetCursorPosCallback(window, Input_CursorPosCallback);
	}

	void Input::PrintActions() const
	{
		for (const auto& [action, bindings] : impl->m_ActionBindings)
		{
			std::cout << "Action: " << action << std::endl;
			for (const auto& binding : bindings)
			{
				std::cout << "Binding Type: " << (binding.type == Binding::Type::Key ? "Key" : "MouseButton") << ", Code: " << binding.code << std::endl;
			}
		}
	}
	
	void Input::BindKey(const std::string& action, int key)
	{
		impl->m_ActionBindings[action].push_back({Binding::Type::Key, key});
	}

	void Input::BindMouseButton(const std::string& action, int button)
	{
		impl->m_ActionBindings[action].push_back({Binding::Type::MouseButton, button});
	}

	void Input::SubscribeActionPressed(const std::string& action, ActionCallback callback)
	{
		impl->m_ActionPressedCallbacks[action].push_back(callback);
	}

	void Input::SubscribeActionReleased(const std::string& action, ActionCallback callback)
	{
		impl->m_ActionReleasedCallbacks[action].push_back(callback);
	}
	
	void Input::ProcessEvents()
	{
		while (!impl->m_EventQueue.empty())
		{
			impl->HandleEvent(impl->m_EventQueue.front());
			impl->m_EventQueue.pop_front();
		}
	}

	void Input::PushEvent(const InputEvent& event)
	{
		impl->PushEvent(event);
	}
}

extern "C"
{
	INPUT_API void Input_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		std::cout << "Input: KeyCallback" << std::endl;
		Nightbird::Input::Get().PushEvent({Nightbird::InputEvent::Type::Key, key, action, 0.0, 0.0});
	}

	INPUT_API void Input_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		Nightbird::Input::Get().PushEvent({Nightbird::InputEvent::Type::MouseButton, button, action, 0.0, 0.0});
	}

	INPUT_API void Input_CursorPosCallback(GLFWwindow* window, double x, double y)
	{
		Nightbird::Input::Get().PushEvent({Nightbird::InputEvent::Type::MouseMove, 0, 0, x, y});
	}
}