#include "audio/player/AudioSystem.h"

#include "audio/player/backends/AudioBackend.h"
#include "audio/player/Defines.h"

uaudio::player::AudioSystem uaudio::player::audioSystem;

namespace uaudio
{
	namespace player
	{
		AudioSystem::AudioSystem()
		{
			m_AudioBackend = new AudioBackend();
		}

		uint32_t AudioSystem::GetBufferSize() const
		{
			return m_AudioBackend->GetBufferSize();
		}

		void AudioSystem::SetBufferSize(uint32_t a_BufferSize)
		{
			m_AudioBackend->SetBufferSize(a_BufferSize);
		}

		void AudioSystem::SetBufferSize(BUFFERSIZE a_BufferSize)
		{
			m_AudioBackend->SetBufferSize(a_BufferSize);
		}

		bool AudioSystem::IsPaused() const
		{
			return m_AudioBackend->IsPaused();
		}

		void AudioSystem::SetPaused(bool a_Paused)
		{
			m_AudioBackend->SetPaused(a_Paused);
		}

		float AudioSystem::GetVolume() const
		{
			return m_AudioBackend->GetVolume();
		}

		void AudioSystem::SetVolume(float a_Volume)
		{
			m_AudioBackend->SetVolume(a_Volume);
		}

		float AudioSystem::GetPanning() const
		{
			return m_AudioBackend->GetPanning();
		}

		void AudioSystem::SetPanning(float a_Panning)
		{
			m_AudioBackend->SetPanning(a_Panning);
		}
	}
}