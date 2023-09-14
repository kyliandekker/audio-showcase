#pragma once

#include "audio/player/UAUDIO_PLAYER_RESULT.h"

namespace uaudio
{
	namespace player
	{
		constexpr float UAUDIO_MIN_VOLUME = 0.0f;
		constexpr float UAUDIO_MAX_VOLUME = 1.0f;

		constexpr float UAUDIO_MIN_PANNING = -1.0f;
		constexpr float UAUDIO_MAX_PANNING = 1.0f;

		constexpr float UAUDIO_DEFAULT_VOLUME = UAUDIO_MAX_VOLUME;
		constexpr float UAUDIO_DEFAULT_PANNING = 0.0f;

		enum class TIMEUNIT
		{
			TIMEUNIT_MS,
			TIMEUNIT_S,
			TIMEUNIT_POS
		};
	}
}