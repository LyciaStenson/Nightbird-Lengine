#include "Glfw/InputProvider.h"

namespace Nightbird::Glfw
{
	InputProvider::InputProvider(GLFWwindow* window)
		: m_Window(window)
	{

	}

	void InputProvider::Poll(Input::State& state)
	{
		state.keysPressed.fill(false);
		state.keysReleased.fill(false);
		state.buttonsPressed.fill(false);
		state.buttonsReleased.fill(false);

		PollKeyboard(state);
		PollGamepad(state);
	}

	void InputProvider::PollKeyboard(Input::State& state)
	{
		static const int glfwKeys[] = {
			GLFW_KEY_SPACE, GLFW_KEY_ENTER, GLFW_KEY_ESCAPE,
			GLFW_KEY_A, GLFW_KEY_B, GLFW_KEY_C, GLFW_KEY_D, GLFW_KEY_E,
			GLFW_KEY_F, GLFW_KEY_G, GLFW_KEY_H, GLFW_KEY_I, GLFW_KEY_J,
			GLFW_KEY_K, GLFW_KEY_L, GLFW_KEY_M, GLFW_KEY_N, GLFW_KEY_O,
			GLFW_KEY_P, GLFW_KEY_Q, GLFW_KEY_R, GLFW_KEY_S, GLFW_KEY_T,
			GLFW_KEY_U, GLFW_KEY_V, GLFW_KEY_W, GLFW_KEY_X, GLFW_KEY_Y,
			GLFW_KEY_Z
		};

		for (int glfwKey : glfwKeys)
		{
			Input::Key key = ResolveGlfwKey(glfwKey);
			bool isDown = glfwGetKey(m_Window, glfwKey) == GLFW_PRESS;
			bool wasDown = state.keysDown[static_cast<size_t>(key)];

			state.keysDown[static_cast<size_t>(key)] = isDown;

			if (isDown && !wasDown)
				state.keysPressed[static_cast<size_t>(key)] = true;
			else if (!isDown && wasDown)
				state.keysReleased[static_cast<size_t>(key)] = true;
		}
	}

	void InputProvider::PollGamepad(Input::State& state)
	{
		if (!glfwJoystickIsGamepad(GLFW_JOYSTICK_1))
			return;

		GLFWgamepadstate gamepadState;
		if (!glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadState))
			return;
		
		static const int glfwButtons[] = {
			GLFW_GAMEPAD_BUTTON_A, GLFW_GAMEPAD_BUTTON_B,
			GLFW_GAMEPAD_BUTTON_X, GLFW_GAMEPAD_BUTTON_Y,
			GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER,
			GLFW_GAMEPAD_BUTTON_LEFT_THUMB, GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,
			GLFW_GAMEPAD_BUTTON_START, GLFW_GAMEPAD_BUTTON_BACK,
			GLFW_GAMEPAD_BUTTON_GUIDE,
			GLFW_GAMEPAD_BUTTON_DPAD_UP, GLFW_GAMEPAD_BUTTON_DPAD_DOWN,
			GLFW_GAMEPAD_BUTTON_DPAD_LEFT, GLFW_GAMEPAD_BUTTON_DPAD_RIGHT
		};

		for (int glfwButton : glfwButtons)
		{
			Input::Button button = ResolveGlfwButton(glfwButton);
			bool isDown = gamepadState.buttons[glfwButton] == GLFW_PRESS;
			bool wasDown = state.buttonsDown[static_cast<size_t>(button)];

			state.buttonsDown[static_cast<size_t>(button)] = isDown;

			if (isDown && !wasDown)
				state.buttonsPressed[static_cast<size_t>(button)] = true;
			else if (!isDown && wasDown)
				state.buttonsReleased[static_cast<size_t>(button)] = true;
		}

		state.axes[static_cast<size_t>(Input::Axis::Left)] = { gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_X], gamepadState.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] };
		state.axes[static_cast<size_t>(Input::Axis::Right)] = { gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_X], gamepadState.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] };
	}

	Input::Key InputProvider::ResolveGlfwKey(int glfwKey)
	{
		switch (glfwKey)
		{
			case GLFW_KEY_SPACE:
				return Input::Key::Space;
			case GLFW_KEY_ENTER:
				return Input::Key::Enter;
			case GLFW_KEY_ESCAPE:
				return Input::Key::Escape;
			case GLFW_KEY_A:
				return Input::Key::A;
			case GLFW_KEY_B:
				return Input::Key::B;
			case GLFW_KEY_C:
				return Input::Key::C;
			case GLFW_KEY_D:
				return Input::Key::D;
			case GLFW_KEY_E:
				return Input::Key::E;
			case GLFW_KEY_F:
				return Input::Key::F;
			case GLFW_KEY_G:
				return Input::Key::G;
			case GLFW_KEY_H:
				return Input::Key::H;
			case GLFW_KEY_I:
				return Input::Key::I;
			case GLFW_KEY_J:
				return Input::Key::J;
			case GLFW_KEY_K:
				return Input::Key::K;
			case GLFW_KEY_L:
				return Input::Key::L;
			case GLFW_KEY_M:
				return Input::Key::M;
			case GLFW_KEY_N:
				return Input::Key::N;
			case GLFW_KEY_O:
				return Input::Key::O;
			case GLFW_KEY_P:
				return Input::Key::P;
			case GLFW_KEY_Q:
				return Input::Key::Q;
			case GLFW_KEY_R:
				return Input::Key::R;
			case GLFW_KEY_S:
				return Input::Key::S;
			case GLFW_KEY_T:
				return Input::Key::T;
			case GLFW_KEY_U:
				return Input::Key::U;
			case GLFW_KEY_V:
				return Input::Key::V;
			case GLFW_KEY_W:
				return Input::Key::W;
			case GLFW_KEY_X:
				return Input::Key::X;
			case GLFW_KEY_Y:
				return Input::Key::Y;
			case GLFW_KEY_Z:
				return Input::Key::Z;
			default:
				return Input::Key::UNKNOWN;
		}
	}

	Input::Button InputProvider::ResolveGlfwButton(int glfwButton)
	{
		switch (glfwButton)
		{
		case GLFW_GAMEPAD_BUTTON_A:
			return Input::Button::A;
		case GLFW_GAMEPAD_BUTTON_B:
			return Input::Button::B;
		case GLFW_GAMEPAD_BUTTON_X:
			return Input::Button::X;
		case GLFW_GAMEPAD_BUTTON_Y:
			return Input::Button::Y;
		case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:
			return Input::Button::L;
		case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:
			return Input::Button::R;
		case GLFW_GAMEPAD_BUTTON_LEFT_THUMB:
			return Input::Button::ZL;
		case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:
			return Input::Button::ZR;
		case GLFW_GAMEPAD_BUTTON_START:
			return Input::Button::Start;
		case GLFW_GAMEPAD_BUTTON_BACK:
			return Input::Button::Back;
		case GLFW_GAMEPAD_BUTTON_GUIDE:
			return Input::Button::Home;
		case GLFW_GAMEPAD_BUTTON_DPAD_UP:
			return Input::Button::Up;
		case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:
			return Input::Button::Down;
		case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:
			return Input::Button::Left;
		case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:
			return Input::Button::Right;
		default:
			return Input::Button::UNKNOWN;
		}
	}
}