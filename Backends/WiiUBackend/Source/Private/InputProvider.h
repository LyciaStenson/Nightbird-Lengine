#pragma once

#include "Input/Provider.h"
#include "Input/State.h"

#include <vpad/input.h>

namespace Nightbird::WiiU
{
	class InputProvider : public Input::Provider
	{
	public:
		InputProvider() = default;
		~InputProvider() = default;

		void Poll(Input::State& state) override;

	private:
		void PollGamepad(Input::State& state);

		static Input::Button ResolveVpadButton(uint32_t vpadButton);
	};
}