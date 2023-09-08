#pragma once

#include <cstdint>
#include <thread>

#include "audio/player/Defines.h"
#include "UAUDIO_PLAYER_RESULT.h"
#include <mutex>

namespace uaudio
{
	namespace storage
	{
		class Sound;
	}
	namespace player
	{
		class AudioBackend;
		class AudioChannel;
		struct ChannelHandle;

		class AudioSystem
		{
		public:
			AudioSystem();
			~AudioSystem();

			UAUDIO_PLAYER_RESULT GetBufferSize(uint32_t& a_BufferSize) const;
			UAUDIO_PLAYER_RESULT SetBufferSize(uint32_t a_BufferSize);

			UAUDIO_PLAYER_RESULT IsPaused(bool& a_IsPaused) const;
			UAUDIO_PLAYER_RESULT SetPaused(bool a_Paused);

			UAUDIO_PLAYER_RESULT GetVolume(float& a_Volume) const;
			UAUDIO_PLAYER_RESULT SetVolume(float a_Volume);

			UAUDIO_PLAYER_RESULT GetPanning(float& a_Panning) const;
			UAUDIO_PLAYER_RESULT SetPanning(float a_Panning);

			UAUDIO_PLAYER_RESULT GetEnabled(bool& a_Enabled);

			UAUDIO_PLAYER_RESULT Start();
			UAUDIO_PLAYER_RESULT Stop();

			UAUDIO_PLAYER_RESULT Play(storage::Sound& a_Sound, ChannelHandle& a_Handle);

			UAUDIO_PLAYER_RESULT NumChannels(size_t& a_NumChannels) const;
			UAUDIO_PLAYER_RESULT GetChannel(ChannelHandle& a_Handle, AudioChannel*& a_Channel);

			UAUDIO_PLAYER_RESULT RemoveSound(storage::Sound& a_Sound);

			std::mutex m_Update;
		private:
			UAUDIO_PLAYER_RESULT Update();

			AudioBackend* m_AudioBackend = nullptr;
			bool m_Enabled = false;

			std::thread m_AudioThread;
		};
		extern AudioSystem audioSystem;
	}
}