#include "audio/player/backends/wasapi/WasAPIBackend.h"

#define COBJMACROS
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "audio/storage/Sound.h"
#include "audio/player/ChannelHandle.h"
#include "utils/Logger.h"

namespace uaudio
{
	namespace player
	{
		namespace wasapi
		{
			WasAPIBackend::WasAPIBackend()
			{
				HRESULT hr = CoInitializeEx(nullptr, COINIT_SPEED_OVER_MEMORY);
				if (FAILED(hr))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "<WasAPI> Initializing COM library failed.");
					return;
				}

				IMMDeviceEnumerator* deviceEnumerator;
				hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator), (LPVOID*)(&deviceEnumerator));
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Device Enumerator creation failed.");
					return;
				}

				hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &m_Device);
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Getting default audio endpoint failed.");
					return;
				}

				deviceEnumerator->Release();

				for (size_t i = 0; i < 20; i++)
					m_Channels.push_back(WasAPIChannel(*this));
			}

			WasAPIBackend::~WasAPIBackend()
			{
				m_Channels.clear();

				m_Device->Release();
			}

			void WasAPIBackend::Update()
			{
				for (int32_t i = static_cast<int32_t>(m_Channels.size() - 1); i > -1; i--)
					m_Channels[i].Update();
			}

			UAUDIO_PLAYER_RESULT WasAPIBackend::Play(storage::Sound& a_WaveFile, ChannelHandle& a_Handle)
			{
				// First look for inactive channels.
				for (uint32_t i = 0; i < m_Channels.size(); i++)
				{
					bool isInUse = false;
					m_Channels[i].IsInUse(isInUse);
					if (!isInUse)
					{
						m_Channels[i].SetSound(a_WaveFile);
						m_Channels[i].Play();
						a_Handle = static_cast<int32_t>(i);
						return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
					}
				}

				a_Handle = CHANNEL_NULL_HANDLE;
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FREE_CHANNEL;
			}

			size_t WasAPIBackend::NumChannels() const
			{
				return m_Channels.size();
			}

			AudioChannel* WasAPIBackend::GetChannel(ChannelHandle& a_Handle)
			{
				if (a_Handle == CHANNEL_NULL_HANDLE)
					return nullptr;

				if (static_cast<size_t>(a_Handle) >= m_Channels.size())
					return nullptr;

				return &m_Channels[a_Handle];
			}

			void WasAPIBackend::RemoveSound(storage::Sound& a_Sound)
			{
			}

			IMMDevice& WasAPIBackend::GetDevice() const
			{
				return *m_Device;
			}
		}
	}
}