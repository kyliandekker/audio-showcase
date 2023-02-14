#pragma once

#include <cstdint>

#include "audio/player/Defines.h"

namespace uaudio
{
	namespace player
	{
		class AudioBackend;

		class AudioSystem
		{
		public:
			AudioSystem();
			uint32_t GetBufferSize() const;
			void SetBufferSize(uint32_t a_BufferSize);
			void SetBufferSize(BUFFERSIZE a_BufferSize);

			bool IsPaused() const;
			void SetPaused(bool a_Paused);

			float GetVolume() const;
			void SetVolume(float a_Volume);

			float GetPanning() const;
			void SetPanning(float a_Panning);

		private:
			AudioBackend* m_AudioBackend = nullptr;
		};
		extern AudioSystem audioSystem;
	}
}