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

		UAUDIO_PLAYER_RESULT AudioSystem::GetBufferSize(uint32_t& a_BufferSize) const
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			a_BufferSize = m_AudioBackend->GetBufferSize();
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::SetBufferSize(uint32_t a_BufferSize)
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;


			m_AudioBackend->SetBufferSize(a_BufferSize);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::IsPaused(bool& a_IsPaused) const
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			a_IsPaused = m_AudioBackend->IsPaused();
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::SetPaused(bool a_Paused)
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			m_AudioBackend->SetPaused(a_Paused);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::GetVolume(float& a_Volume) const
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			a_Volume = m_AudioBackend->GetVolume();
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::SetVolume(float a_Volume)
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			m_AudioBackend->SetVolume(a_Volume);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::GetPanning(float& a_Panning) const
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			a_Panning = m_AudioBackend->GetPanning();
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::SetPanning(float a_Panning)
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			m_AudioBackend->SetPanning(a_Panning);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::Update()
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}
	}
}