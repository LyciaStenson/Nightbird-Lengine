#pragma once

#include "Input/Provider.h"
#include "Input/State.h"

namespace Nightbird::N3DS
{
	class InputProvider : public Input::Provider
	{
	public:
		void Poll(Input::State& state) override;
	};
}
