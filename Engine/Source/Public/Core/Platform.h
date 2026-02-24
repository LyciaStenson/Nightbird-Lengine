#pragma once

namespace Nightbird
{
	class Platform
	{
	public:
		virtual ~Platform() = default;

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;
		virtual void PollEvents() = 0;
		virtual void WaitEvents() = 0;
		virtual bool ShouldClose() const = 0;
		virtual void GetFramebufferSize(int* width, int* height) const = 0;
	};
}