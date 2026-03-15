#include "InputProvider.h"

namespace Nightbird::WiiU
{
	void InputProvider::Poll(Input::State& state)
	{
		state.buttonsPressed.fill(false);
		state.buttonsReleased.fill(false);

		PollGamepad(state);
	}

	void InputProvider::PollGamepad(Input::State& state)
	{
		VPADStatus vpadStatus;
		VPADReadError vpadError;

		VPADRead(VPAD_CHAN_0, &vpadStatus, 1, &vpadError);

		if (vpadError != VPAD_READ_SUCCESS)
			return;

		static const uint32_t vpadButtons[] =
		{
			VPAD_BUTTON_A, VPAD_BUTTON_B, VPAD_BUTTON_X, VPAD_BUTTON_Y,
			VPAD_BUTTON_L, VPAD_BUTTON_R, VPAD_BUTTON_ZL, VPAD_BUTTON_ZR,
			VPAD_BUTTON_PLUS, VPAD_BUTTON_MINUS, VPAD_BUTTON_HOME,
			VPAD_BUTTON_UP, VPAD_BUTTON_DOWN, VPAD_BUTTON_LEFT, VPAD_BUTTON_RIGHT
		};

		for (uint32_t vpadButton : vpadButtons)
		{
			Input::Button button = ResolveVpadButton(vpadButton);
			bool isDown = vpadStatus.hold & vpadButton;
			bool wasDown = state.buttonsDown[static_cast<size_t>(button)];

			state.buttonsDown[static_cast<size_t>(button)] = isDown;

			if (isDown && !wasDown)
				state.buttonsPressed[static_cast<size_t>(button)] = true;
			else if (!isDown && wasDown)
				state.buttonsReleased[static_cast<size_t>(button)] = true;
		}

		state.axes[static_cast<size_t>(Input::Axis::Left)] = { vpadStatus.leftStick.x, vpadStatus.leftStick.y };
		state.axes[static_cast<size_t>(Input::Axis::Right)] = { vpadStatus.rightStick.x, vpadStatus.rightStick.y };
	}

	Input::Button InputProvider::ResolveVpadButton(uint32_t vpadButton)
	{
		switch (vpadButton)
		{
			case VPAD_BUTTON_A:
				return Input::Button::A;
			case VPAD_BUTTON_B:
				return Input::Button::B;
			case VPAD_BUTTON_X:
				return Input::Button::X;
			case VPAD_BUTTON_Y:
				return Input::Button::Y;
			case VPAD_BUTTON_L:
				return Input::Button::L;
			case VPAD_BUTTON_R:
				return Input::Button::R;
			case VPAD_BUTTON_ZL:
				return Input::Button::ZL;
			case VPAD_BUTTON_ZR:
				return Input::Button::ZR;
			case VPAD_BUTTON_PLUS:
				return Input::Button::Plus;
			case VPAD_BUTTON_MINUS:
				return Input::Button::Minus;
			case VPAD_BUTTON_HOME:
				return Input::Button::Home;
			case VPAD_BUTTON_UP:
				return Input::Button::Up;
			case VPAD_BUTTON_DOWN:
				return Input::Button::Down;
			case VPAD_BUTTON_LEFT:
				return Input::Button::Left;
			case VPAD_BUTTON_RIGHT:
				return Input::Button::Right;
			default:
				return Input::Button::UNKNOWN;
		}
	}
}