#pragma once

#include <cstdint>

#include "audio/player/UAUDIO_PLAYER_RESULT.h"
#include "audio/player/Defines.h"

namespace uaudio
{
	namespace storage
	{
		class Sound;
	}
	namespace player
	{
		class AudioChannel;
		struct ChannelHandle;

		class AudioBackend
		{
		public:
			uint32_t GetBufferSize() const { return m_BufferSize; }
			void SetBufferSize(uint32_t a_BufferSize) { m_BufferSize = a_BufferSize; }

			bool IsPaused() const { return m_Paused; }
			void SetPaused(bool a_Paused) { m_Paused = a_Paused; }

			float GetVolume() const { return m_Volume; }
			void SetVolume(float a_Volume) { m_Volume = a_Volume; }

			float GetPanning() const { return m_Panning; }
			void SetPanning(float a_Panning) { m_Panning = a_Panning; }

			virtual size_t NumChannels() const = 0;

			virtual UAUDIO_PLAYER_RESULT Play(storage::Sound& a_WaveFile, ChannelHandle& a_Handle) = 0;

			virtual void Update();

			virtual AudioChannel* GetChannel(ChannelHandle& a_Handle) = 0;
			virtual void RemoveSound(storage::Sound& a_Sound) = 0;
		protected:
			uint32_t m_BufferSize = 4096;
			bool m_Paused = false;
			float m_Volume = UAUDIO_DEFAULT_VOLUME;
			float m_Panning = UAUDIO_DEFAULT_PANNING;
		};
	}
}