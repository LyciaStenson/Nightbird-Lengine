#pragma once

#include "Core/Reflection.h"

#include <vector>

#ifdef NB_EDITOR_BUILD
#include <uuid.h>
#include <array>
#endif

namespace Nightbird::Core
{
	class Cubemap
	{
	public:
		NB_TYPE_BASE()
		Cubemap() = default;
		Cubemap(uint32_t faceSize, std::vector<uint8_t> data);

		uint32_t GetFaceSize() const;

		const std::vector<uint8_t>& GetData() const;
		void DiscardData();
		bool HasData() const;

#ifdef NB_EDITOR_BUILD
		std::array<uuids::uuid, 6> m_FaceUUIDs;
#endif

	private:
		uint32_t m_FaceSize;
		std::vector<uint8_t> m_Data;
	};
}