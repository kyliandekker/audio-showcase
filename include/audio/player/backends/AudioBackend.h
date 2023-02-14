#pragma once

#include <cstdint>
#include <audio/player/Defines.h>

namespace uaudio
{
	namespace player
	{
		class AudioBackend
		{
		public:
			uint32_t GetBufferSize() const { return m_BufferSize; }
			void SetBufferSize(uint32_t a_BufferSize) { m_BufferSize = a_BufferSize; }
			void SetBufferSize(BUFFERSIZE a_BufferSize) { m_BufferSize = static_cast<uint32_t>(a_BufferSize); }

			bool IsPaused() const { return m_Paused; }
			void SetPaused(bool a_Paused) { m_Paused = a_Paused; }

			float GetVolume() const { return m_Volume; }
			void SetVolume(float a_Volume) { m_Volume = a_Volume; }

			float GetPanning() const { return m_Panning; }
			void SetPanning(float a_Panning) { m_Panning = a_Panning; }
		private:
			uint32_t m_BufferSize;
			bool m_Paused = false;
			float m_Volume = UAUDIO_DEFAULT_VOLUME;
			float m_Panning = UAUDIO_DEFAULT_PANNING;
		};
	}
}