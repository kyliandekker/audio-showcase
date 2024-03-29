#include "audio/player/backends/xaudio2/XAudio2Backend.h"
#include <xaudio2.h>

#include "audio/player/ChannelHandle.h"
#include "utils/Logger.h"

namespace uaudio
{
	namespace player
	{
		namespace xaudio2
		{
			XAudio2Backend::~XAudio2Backend()
			{
				m_Channels.clear();

				m_MasteringVoice->DestroyVoice();
				m_Engine->Release();

				CoUninitialize();
			}

			XAudio2Backend::XAudio2Backend()
			{
				HRESULT hr;
				if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED)))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "<XAudio2> Initializing COM library failed.");
					return;
				}

				m_Engine = nullptr;
				if (FAILED(hr = XAudio2Create(&m_Engine, 0, XAUDIO2_DEFAULT_PROCESSOR)))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<XAudio2> XAudio2 Engine creation failed.");
					return;
				}

				m_MasteringVoice = nullptr;
				if (FAILED(hr = m_Engine->CreateMasteringVoice(&m_MasteringVoice)))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<XAudio2> XAudio2 Mastering Voice creation failed.");
					return;
				}

				for (size_t i = 0; i < 20; i++)
					m_Channels.push_back(XAudio2Channel(*this));

			}

			void XAudio2Backend::Update()
			{
				if (m_Paused)
					return;

				for (int32_t i = static_cast<int32_t>(m_Channels.size() - 1); i > -1; i--)
					m_Channels[i].Update();
			}

			UAUDIO_PLAYER_RESULT XAudio2Backend::Play(storage::Sound& a_Sound, ChannelHandle& a_Handle)
			{
				// First look for inactive channels.
				for (uint32_t i = 0; i < m_Channels.size(); i++)
				{
					XAudio2Channel& channel = m_Channels[i];

					if (channel.m_Sound == nullptr || (channel.m_Sound == &a_Sound && channel.m_Sound != nullptr && channel.m_CurrentPos == 0 && !channel.m_IsPlaying))
					{
						channel.SetSound(a_Sound);
						channel.Play();
						a_Handle = static_cast<int32_t>(i);
						return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
					}
				}

				a_Handle = CHANNEL_NULL_HANDLE;
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FREE_CHANNEL;
			}

			IXAudio2& XAudio2Backend::GetEngine() const
			{
				return *m_Engine;
			}

			IXAudio2MasteringVoice& XAudio2Backend::GetMasteringVoice() const
			{
				return *m_MasteringVoice;
			}

			size_t XAudio2Backend::NumChannels() const
			{
				return m_Channels.size();
			}

			AudioChannel* XAudio2Backend::GetChannel(ChannelHandle& a_Handle)
			{
				if (a_Handle == CHANNEL_NULL_HANDLE)
					return nullptr;

				if (static_cast<size_t>(a_Handle) >= m_Channels.size())
					return nullptr;

				return &m_Channels[a_Handle];
			}

			void XAudio2Backend::RemoveSound(storage::Sound& a_Sound)
			{
				for (size_t i = 0; i < m_Channels.size(); i++)
				{
					XAudio2Channel& channel = m_Channels[i];

					if (channel.m_Sound == &a_Sound)
						channel.RemoveSound();
				}
			}
		}
	}
}