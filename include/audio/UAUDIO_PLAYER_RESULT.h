#pragma once

namespace uaudio
{
	namespace player
	{
		enum class UAUDIO_PLAYER_RESULT
		{
			UAUDIO_OK,
			UAUDIO_ERR_BITS_PER_SAMPLE_UNSUPPORTED,
			UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE,
			UAUDIO_BAD_SOURCEBUFFER_MAX_REACHED,
			UAUDIO_UNKNOWN_ERROR,
			UAUDIO_ERR_SOUND_NOT_SET,
			UAUDIO_ERR_NO_FMT_CHUNK,
			UAUDIO_ERR_NO_DATA_CHUNK,
			UAUDIO_ERR_XAUDIO2_NO_SYSTEM,
			UAUDIO_ERR_XAUDIO2_SOURCEVOICE_CREATION_FAILED,
			UAUDIO_ERR_CHANNEL_NOT_PLAYING,
			UAUDIO_ERR_NUM_CHANNELS_INVALID,
		};
	}
}