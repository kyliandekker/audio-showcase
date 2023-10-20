#include "audio/player/AudioSystem.h"

#include "audio/player/backends/AudioBackend.h"
#include "audio/player/backends/xaudio2/XAudio2Backend.h"
#include "audio/player/backends/wasapi/WasAPIBackend.h"
#include "audio/player/ChannelHandle.h"
#include "audio/storage/Sound.h"
#include "audio/player/Defines.h"
#include "utils/Logger.h"
#include <iostream>

#include "windows.h"

uaudio::player::AudioSystem uaudio::player::audioSystem;

namespace uaudio
{
	namespace player
	{
		AudioSystem::AudioSystem()
		{
			SetBackend(m_Backend);
		}

		AudioSystem::~AudioSystem()
		{
			m_Enabled = false;
			m_AudioThread.join();
			delete m_AudioBackend;
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
				std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

				m_Update.lock();
				m_AudioBackend->Update();
				m_Update.unlock();

				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

				double to_sleep = (1000.0 / 60.0);

				m_DeltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count()) / 1000000.0;
				to_sleep -= m_DeltaTime;
				double total = m_DeltaTime / 1000.0;

				if (to_sleep > 0.0)
				{
					total += to_sleep / 1000.0;
					Sleep(static_cast<DWORD>(to_sleep));
				}

				m_DeltaTime = total;

				m_Frames++;
				m_Time += m_DeltaTime;

				m_Fps = (m_Frames / (m_Time / 1000)) / 1000;
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
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_CHANNEL_DOES_NOT_EXIST;

			if (static_cast<size_t>(a_Handle) >= m_AudioBackend->NumChannels())
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_CHANNEL_DOES_NOT_EXIST;

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

		UAUDIO_PLAYER_RESULT AudioSystem::SetBackend(Backend a_Backend)
		{
			m_Update.lock();
			m_Backend = a_Backend;

			if (m_AudioBackend)
				delete m_AudioBackend;

			switch (a_Backend)
			{
				case Backend::XAUDIO2:
				{
					m_AudioBackend = new xaudio2::XAudio2Backend();
					break;
				}
				case Backend::WASAPI:
				{
					m_AudioBackend = new wasapi::WasAPIBackend();
					break;
				}
			}
			m_Update.unlock();
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioSystem::GetBackend(Backend& a_Backend)
		{
			a_Backend = m_Backend;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}
	}
}