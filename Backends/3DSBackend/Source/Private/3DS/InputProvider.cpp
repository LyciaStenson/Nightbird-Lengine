#include "3DS/InputProvider.h"

#include <3ds.h>

namespace Nightbird::N3DS
{
	void InputProvider::Poll(Input::State& state)
	{
		state.buttonsPressed.fill(false);
		state.buttonsReleased.fill(false);

		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		u32 kUp = hidKeysUp();

		auto processButton = [&](Input::Button button, u32 key)
		{
			size_t idx = static_cast<size_t>(button);
			bool isDown = kHeld & key;
			//bool wasDown = state.buttonsDown[idx];
			state.buttonsDown[idx] = isDown;
			if (kDown & key)
				state.buttonsPressed[idx] = true;
			if (kUp & key)
				state.buttonsReleased[idx] = true;
		};

		processButton(Input::Button::A, KEY_A);
		processButton(Input::Button::B, KEY_B);
		processButton(Input::Button::X, KEY_X);
		processButton(Input::Button::Y, KEY_Y);
		processButton(Input::Button::L, KEY_L);
		processButton(Input::Button::R, KEY_R);
		processButton(Input::Button::Start, KEY_START);
		processButton(Input::Button::Select, KEY_SELECT);
		processButton(Input::Button::Up, KEY_DUP);
		processButton(Input::Button::Down, KEY_DDOWN);
		processButton(Input::Button::Left, KEY_DLEFT);
		processButton(Input::Button::Right, KEY_DRIGHT);

		circlePosition circlePos;
		hidCircleRead(&circlePos);
		state.axes[static_cast<size_t>(Input::Axis::Left)] = { circlePos.dx / 156.0f, circlePos.dy / 156.0f };
	}
}
