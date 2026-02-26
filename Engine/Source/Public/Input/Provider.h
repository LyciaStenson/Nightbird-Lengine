#pragma once

#include "Input/State.h"

namespace Nightbird::Input
{
	class Provider
	{
	public:
		virtual ~Provider() = default;
		virtual void Poll(State& state) = 0;
	};
}