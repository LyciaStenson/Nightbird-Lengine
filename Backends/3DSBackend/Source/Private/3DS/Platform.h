#pragma once

#include "Core/Platform.h"

#include "3DS/InputProvider.h"

#include <string>

namespace Nightbird::N3DS
{
	class Platform : public Core::Platform
	{
	public:
		Platform();
		virtual void Initialize() override;
		virtual void Shutdown() override;
		virtual void PollEvents() override;
		virtual void WaitEvents() override;
		virtual bool ShouldClose() const override;
		virtual void GetFramebufferSize(int* width, int* height) const override;

		virtual std::string GetCookedAssetsPath() const override;

		Input::Provider& GetInputProvider() override;

	private:
		InputProvider m_InputProvider;
	};
}
