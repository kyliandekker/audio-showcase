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

		enum class BUFFERSIZE
		{
			BUFFERSIZE_256 = 256,
			BUFFERSIZE_384 = 384,
			BUFFERSIZE_512 = 512,
			BUFFERSIZE_1024 = 1024,
			BUFFERSIZE_2048 = 2048,
			BUFFERSIZE_4096 = 4096,
			BUFFERSIZE_8192 = 8192,
		};
	}
}