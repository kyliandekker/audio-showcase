#include "audio/player/backends/wasapi/WasAPIChannel.h"

#define COBJMACROS
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <cassert>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/UAUDIO_WAVE_READER_RESULT.h>

#include "audio/player/backends/wasapi/WasAPIBackend.h"
#include "audio/storage/Sound.h"
#include "utils/Logger.h"
#include "audio/player/AudioSystem.h"

namespace uaudio
{
	namespace player
	{
		namespace wasapi
		{
			WasAPIChannel::WasAPIChannel()
			{
				HRESULT hr = m_Backend->GetDevice().Activate(__uuidof(IAudioClient2), CLSCTX_ALL, nullptr, (LPVOID*)(&m_AudioClient));
				if (FAILED(hr))
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Audio client activation failed.");
			}

			WasAPIChannel::WasAPIChannel(WasAPIBackend& a_Backend) : m_Backend(&a_Backend)
			{ }

			WasAPIChannel::~WasAPIChannel()
			{
				Stop();
				if (m_AudioClient)
				{
					m_AudioClient->Stop();
					m_AudioClient->Release();
				}
				if (m_RenderClient)
					m_RenderClient->Release();
			}

			UAUDIO_PLAYER_RESULT WasAPIChannel::SetSound(storage::Sound& a_Sound)
			{
				HRESULT hr = m_Backend->GetDevice().Activate(__uuidof(IAudioClient2), CLSCTX_ALL, nullptr, (LPVOID*)(&m_AudioClient));
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Audio client activation failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_ACTIVATION_OF_AUDIO_CLIENT;
				}

				m_CurrentPos = 0;

				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = a_Sound.m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
				if (UAUDIOWAVEREADERFAILED(result))
					return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

				WAVEFORMATEX wave = {};
				wave.wFormatTag = fmt_chunk.audioFormat;
				wave.nChannels = fmt_chunk.numChannels;
				wave.nSamplesPerSec = fmt_chunk.sampleRate;
				wave.cbSize = 0;
				wave.wBitsPerSample = fmt_chunk.bitsPerSample;
				wave.nBlockAlign = fmt_chunk.blockAlign;
				wave.nAvgBytesPerSec = fmt_chunk.byteRate;

				uaudio::wave_reader::DATA_Chunk data_chunk;
				result = a_Sound.m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
				if (result != uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
					return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

				const int64_t REFTIMES_PER_SEC = 10000000; // hundred nanoseconds
				REFERENCE_TIME requestedSoundBufferDuration = REFTIMES_PER_SEC * 2;
				DWORD initStreamFlags = (AUDCLNT_STREAMFLAGS_RATEADJUST
					| AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
					| AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY);
				hr = m_AudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
					initStreamFlags,
					requestedSoundBufferDuration,
					0, &wave, nullptr);
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Audio client initialization failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_INITIALIZING_AUDIO_CLIENT;
				}

				hr = m_AudioClient->GetService(__uuidof(IAudioRenderClient), (LPVOID*)(&m_RenderClient));
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Retrieving audio render client failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_RETRIEVING_RENDER_CLIENT;
				}

				UINT32 bufferSizeInFrames;
				hr = m_AudioClient->GetBufferSize(&bufferSizeInFrames);
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Retrieving buffer size from audio client failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_RETRIEVING_BUFFER;
				}

				hr = m_AudioClient->Start();
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Starting audio client failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_STARTING_AUDIO_CLIENT;
				}

				return AudioChannel::SetSound(a_Sound);
			}

			UAUDIO_PLAYER_RESULT WasAPIChannel::Stop()
			{
				AudioChannel::Stop();
				if (m_AudioClient)
					m_AudioClient->Stop();

				if (m_RenderClient)
					m_RenderClient->Release();

				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}

			UAUDIO_PLAYER_RESULT WasAPIChannel::Update()
			{
				if (m_Sound == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

				if (m_AudioClient == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_NO_AUDIO_CLIENT;
				
				if (m_RenderClient == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_NO_AUDIO_RENDER_CLIENT;

				if (!m_IsPlaying)
					return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
;
				UINT32 bufferPadding;
				HRESULT hr = m_AudioClient->GetCurrentPadding(&bufferPadding);
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Retrieving padding from audio client failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_RETRIEVING_PADDING;
				}

				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_Sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
				if (result != uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
					return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

				double buffersize = fmt_chunk.byteRate * uaudio::player::audioSystem.m_DeltaTime;

				double left_over = floor(fmod(buffersize, fmt_chunk.blockAlign));
				double add = fmt_chunk.blockAlign - left_over;
				double real_buffersize = static_cast<uint32_t>(buffersize) + static_cast<uint32_t>(add);

				if (real_buffersize == 0)
					return UAUDIO_PLAYER_RESULT::UAUDIO_OK;

				UINT32 numFramesToWrite = real_buffersize - bufferPadding;
				PlayRanged(m_CurrentPos, numFramesToWrite);

				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}

			uint32_t wavPlaybackSample = 0;
			UAUDIO_PLAYER_RESULT WasAPIChannel::PlayRanged(uint32_t a_StartPos, uint32_t a_Size)
			{
				if (m_Sound == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

				if (m_AudioClient == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_NO_AUDIO_CLIENT;

				if (m_RenderClient == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_NO_AUDIO_RENDER_CLIENT;

				if (!m_IsPlaying)
					return UAUDIO_PLAYER_RESULT::UAUDIO_OK;

				uint32_t size = 0;
				m_Sound->m_ChunkCollection->GetChunkSize(size, uaudio::wave_reader::DATA_CHUNK_ID);
				// If the sound is done playing, check whether it needs to be repeated or whether it needs to be stopped entirely.
				if (a_StartPos >= size)
				{
					m_CurrentPos = 0;
					a_StartPos = 0;

					// If the sound is not set to repeat, then stop the channel.
					if (!m_Looping)
					{
						Pause();
						SetPos(0);
						m_LastDataSize = 0;
						m_LastPlayedData = nullptr;
						return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
					}
				}

				m_Sound->PreRead(a_StartPos, a_Size);
				unsigned char* data = nullptr, *actual_data = nullptr;
				HRESULT hr = m_RenderClient->GetBuffer(a_Size, &data);
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Retrieving padding from audio client failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_RETRIEVING_BUFFER;
				}

				if (a_Size == 0)
					return UAUDIO_PLAYER_RESULT::UAUDIO_OK;

				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_Sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
				if (UAUDIOWAVEREADERFAILED(result))
					return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

				uint32_t actual_size = a_Size * fmt_chunk.blockAlign;
				m_Sound->Read(m_CurrentPos, actual_size, actual_data);
				for (size_t i = 0; i < actual_size; i++)
					data[i] = actual_data[i];

				m_CurrentPos = a_StartPos;
				m_CurrentPos += actual_size;

				AddEffects(data, actual_size);
				PlayBuffer(data, a_Size);

				m_LastPlayedData = data;
				m_LastDataSize = actual_size;

				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}

			UAUDIO_PLAYER_RESULT WasAPIChannel::PlayBuffer(const unsigned char* a_DataBuffer, uint32_t a_Size) const
			{
				HRESULT hr = m_RenderClient->ReleaseBuffer(a_Size, 0);
				if (FAILED(hr))
				{
					LOG(logger::LOGSEVERITY_ERROR, "<WasAPI> Retrieving padding from audio client failed.");
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_WASAPI_FAILED_RELEASING_BUFFER;
				}

				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}
		}
	}
}