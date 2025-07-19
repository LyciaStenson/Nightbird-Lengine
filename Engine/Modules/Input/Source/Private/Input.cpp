#include <Input.h>

namespace Nightbird
{
	Input* Input::instance = nullptr;

	Input& Input::Get()
	{
		if (!instance)
			instance = new Input();
		return *instance;
	}

	void Input::Shutdown()
	{
		delete instance;
		instance = nullptr;
	}

	void Input::Init(GLFWwindow* window)
	{
		m_Window = window;

		glfwSetKeyCallback(window, &Input::KeyCallback);
		glfwSetMouseButtonCallback(window, &Input::MouseButtonCallback);
		glfwSetCursorPosCallback(window, &Input::CursorPosCallback);
	}
	
	void Input::BindKey(const std::string& action, int key)
	{
		m_ActionBindings[action].push_back({Binding::Type::Key, key});
	}

	void Input::BindMouseButton(const std::string& action, int button)
	{
		m_ActionBindings[action].push_back({Binding::Type::MouseButton, button});
	}

	void Input::SubscribeActionPressed(const std::string& action, ActionCallback callback)
	{
		m_ActionPressedCallbacks[action].push_back(callback);
	}

	void Input::SubscribeActionReleased(const std::string& action, ActionCallback callback)
	{
		m_ActionReleasedCallbacks[action].push_back(callback);
	}

	void Input::PushEvent(const InputEvent& event)
	{
		m_EventQueue.push_back(event);
	}
	
	void Input::HandleEvent(const InputEvent& event)
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
									callback(action);
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
									callback(action);
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

	void Input::ProcessEvents()
	{
		while (!m_EventQueue.empty())
		{
			HandleEvent(m_EventQueue.front());
			m_EventQueue.pop_front();
		}
	}
	
	void Input::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Get().PushEvent({ InputEvent::Type::Key, key, action, 0.0, 0.0 });
	}

	void Input::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		Get().PushEvent({ InputEvent::Type::MouseButton, button, action, 0.0, 0.0 });
	}

	void Input::CursorPosCallback(GLFWwindow* window, double x, double y)
	{
		Get().PushEvent({ InputEvent::Type::MouseMove, 0, 0, x, y });
	}
}