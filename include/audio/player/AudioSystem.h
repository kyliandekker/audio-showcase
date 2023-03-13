#pragma once

#include <cstdint>

#include "audio/player/Defines.h"
#include "UAUDIO_PLAYER_RESULT.h"

namespace uaudio
{
	namespace player
	{
		class AudioBackend;

		class AudioSystem
		{
		public:
			AudioSystem();
			UAUDIO_PLAYER_RESULT GetBufferSize(uint32_t& a_BufferSize) const;
			UAUDIO_PLAYER_RESULT SetBufferSize(uint32_t a_BufferSize);

			UAUDIO_PLAYER_RESULT IsPaused(bool& a_IsPaused) const;
			UAUDIO_PLAYER_RESULT SetPaused(bool a_Paused);

			UAUDIO_PLAYER_RESULT GetVolume(float& a_Volume) const;
			UAUDIO_PLAYER_RESULT SetVolume(float a_Volume);

			UAUDIO_PLAYER_RESULT GetPanning(float& a_Panning) const;
			UAUDIO_PLAYER_RESULT SetPanning(float a_Panning);

			UAUDIO_PLAYER_RESULT Update();

		private:
			AudioBackend* m_AudioBackend = nullptr;
		};
		extern AudioSystem audioSystem;
	}
}