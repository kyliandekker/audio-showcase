#include "audio/player/AudioSystem.h"

#include "audio/player/backends/AudioBackend.h"
#include "audio/player/backends/xaudio2/XAudio2Backend.h"
#include "audio/player/ChannelHandle.h"
#include "audio/player/Defines.h"
#include "utils/Logger.h"

uaudio::player::AudioSystem uaudio::player::audioSystem;

namespace uaudio
{
	namespace player
	{
		AudioSystem::AudioSystem()
		{
			m_AudioBackend = new xaudio2::XAudio2Backend();
		}

		AudioSystem::~AudioSystem()
		{
			m_Enabled = false;
			m_AudioThread.join();
			delete m_AudioBackend;
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
			while (m_Enabled)
			{
				m_Update.lock();
				m_AudioBackend->Update();
				m_Update.unlock();
			}
			LOG(logger::LOGSEVERITY_INFO, "Stopped audio thread.");
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::GetEnabled(bool& a_Enabled)
		{
			a_Enabled = m_Enabled;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::Start()
		{
			m_Enabled = true;

			LOG(logger::LOGSEVERITY_INFO, "Started audio thread.");
			m_AudioThread = std::thread(&AudioSystem::Update, this);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::Stop()
		{
			m_Enabled = false;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::Play(storage::Sound& a_Sound, ChannelHandle& a_Handle)
		{
			return m_AudioBackend->Play(a_Sound, a_Handle);
		}

		UAUDIO_PLAYER_RESULT AudioSystem::NumChannels(size_t& a_NumChannels) const
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			a_NumChannels = m_AudioBackend->NumChannels();
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::GetChannel(ChannelHandle& a_Handle, AudioChannel*& a_Channel)
		{
			if (a_Handle == CHANNEL_NULL_HANDLE)
				return UAUDIO_PLAYER_RESULT::UAUDIO_CHANNEL_DOES_NOT_EXIST;

			if (static_cast<size_t>(a_Handle) >= m_AudioBackend->NumChannels())
				return UAUDIO_PLAYER_RESULT::UAUDIO_CHANNEL_DOES_NOT_EXIST;

			a_Channel = m_AudioBackend->GetChannel(a_Handle);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::RemoveSound(storage::Sound& a_Sound)
		{
			if (m_AudioBackend == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_BACKEND;

			m_AudioBackend->RemoveSound(a_Sound);
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}
	}
}