#include "audio/player/backends/wasapi/WasAPIBackend.h"

#define COBJMACROS
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "audio/storage/Sound.h"

namespace uaudio
{
	namespace player
	{
		namespace wasapi
		{
			WasAPIBackend::WasAPIBackend()
			{
				CoInitializeEx(NULL, 0);

				IMMDeviceEnumerator* enu;
				const GUID _CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
				const GUID _IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
				CoCreateInstance(_CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, _IID_IMMDeviceEnumerator, (void**)&enu);
				enu->Release();

				wchar_t* device_id = NULL;
				if (device_id == NULL)
				{
					int mode = eRender;
					enu->GetDefaultAudioEndpoint(EDataFlow::eRender, ERole::eConsole, &m_Device);
				}

				const GUID _IID_IAudioClient = __uuidof(IAudioClient);
				m_Device->Activate(_IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&m_AudioClient);

				const GUID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);
				HRESULT hr = m_AudioClient->GetService(
					IID_IAudioRenderClient,
					(void**)&m_AudioRenderClient);
				m_AudioClient->Start();
			}

			WasAPIBackend::~WasAPIBackend()
			{
				m_Device->Release();
				m_AudioClient->Release();
				m_AudioRenderClient->Release();
			}

			void WasAPIBackend::Update()
			{

			}

			UAUDIO_PLAYER_RESULT WasAPIBackend::Play(storage::Sound& a_WaveFile, ChannelHandle& a_Handle)
			{
				uaudio::wave_reader::DATA_Chunk data_chunk;
				a_WaveFile.m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
				m_AudioRenderClient->GetBuffer(2048, &data_chunk.data);
				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}

			size_t WasAPIBackend::NumChannels() const
			{
				return size_t();
			}

			AudioChannel* WasAPIBackend::GetChannel(ChannelHandle& a_Handle)
			{
				return nullptr;
			}

			void WasAPIBackend::RemoveSound(storage::Sound& a_Sound)
			{
			}
		}
	}
}