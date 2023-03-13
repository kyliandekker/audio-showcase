#pragma once

namespace uaudio
{
	namespace player
	{
		constexpr float UAUDIO_DEFAULT_VOLUME = 1.0f;
		constexpr float UAUDIO_DEFAULT_PANNING = 0.0f;

		enum class TIMEUNIT
		{
			TIMEUNIT_MS,
			TIMEUNIT_S,
			TIMEUNIT_POS
		};
	}
}