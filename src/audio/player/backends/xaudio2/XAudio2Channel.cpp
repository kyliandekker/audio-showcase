#include "audio/player/backends/xaudio2/XAudio2Channel.h"

#include <xaudio2.h>
#include <comdef.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>

#include "audio/storage/Sound.h"
#include "audio/player/backends/xaudio2/XAudio2Backend.h"
#include "audio/player/AudioSystem.h"
#include "utils/Logger.h"

namespace uaudio
{
	namespace player
	{
		namespace xaudio2
		{
			XAudio2Channel::XAudio2Channel()
			{
			}

			XAudio2Channel::XAudio2Channel(XAudio2Backend& a_Backend) : m_Backend(&a_Backend)
			{ }

			XAudio2Channel::~XAudio2Channel()
			{
				if (m_SourceVoice)
				{
					Stop();
					m_SourceVoice = nullptr;
				}
			}

			UAUDIO_PLAYER_RESULT XAudio2Channel::SetSound(storage::Sound& a_Sound)
			{
				m_CurrentPos = 0;
				HRESULT hr;
				if (m_SourceVoice != nullptr)
					Stop();
				if (m_SourceVoice == nullptr)
				{
					uaudio::wave_reader::FMT_Chunk fmt_chunk;
					uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = a_Sound.m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
					if (result != uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
						return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

					WAVEFORMATEX wave = {};
					wave.wFormatTag = fmt_chunk.audioFormat;
					wave.nChannels = fmt_chunk.numChannels;
					wave.nSamplesPerSec = fmt_chunk.sampleRate;
					wave.cbSize = 0;
					wave.wBitsPerSample = fmt_chunk.bitsPerSample;
					wave.nBlockAlign = fmt_chunk.blockAlign;
					wave.nAvgBytesPerSec = fmt_chunk.byteRate;
					if (FAILED(hr = m_Backend->GetEngine().CreateSourceVoice(&m_SourceVoice, &wave, 0, 2.0f)))
					{
						LOGF(logger::LOGSEVERITY_ERROR, "<XAudio2> Creating XAudio2 Source Voice failed.");
						m_IsPlaying = false;
						return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_XAUDIO2_SOURCEVOICE_CREATION_FAILED;
					}
				}
				if (FAILED(hr = m_SourceVoice->Start(0, 0)))
				{
					LOGF(logger::LOGSEVERITY_ERROR, "<XAudio2> Starting XAudio2 Source Voice failed.");
					m_IsPlaying = false;
					return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_XAUDIO2_SOURCEVOICE_STARTING_FAILED;
				}

				return AudioChannel::SetSound(a_Sound);
			}

			UAUDIO_PLAYER_RESULT XAudio2Channel::Stop()
			{
				while (!m_DataBuffers.empty())
				{
					buffer& buffer = m_DataBuffers.front();
					free(buffer.data);
					m_DataBuffers.pop();
				}
				total_buffer_size = 0;

				// Stop the source voice.
				if (m_SourceVoice != nullptr)
				{
					m_SourceVoice->Stop();
					m_SourceVoice->FlushSourceBuffers();

					m_SourceVoice->DestroyVoice();
					m_SourceVoice = nullptr;
				}
				AudioChannel::Stop();
				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}

			UAUDIO_PLAYER_RESULT XAudio2Channel::PlayRanged(uint32_t a_StartPos, uint32_t a_Size)
			{
				if (m_Sound == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

				if (m_SourceVoice == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE;

				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_Sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
				if (result != uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
					return uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

				while (total_buffer_size >= fmt_chunk.byteRate + a_Size)
				{
					buffer& buffer = m_DataBuffers.front();
					total_buffer_size = total_buffer_size - buffer.size;

					free(buffer.data);
					m_DataBuffers.pop();
				}

				uint32_t size = 0;
				result = m_Sound->m_ChunkCollection->GetChunkSize(size, uaudio::wave_reader::DATA_CHUNK_ID);
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

				unsigned char* data = {};

				// Read the part of the wave file and store it back in the read buffer.
				m_Sound->Read(a_StartPos, a_Size, data);

				unsigned char* new_data = reinterpret_cast<unsigned char*>(malloc(a_Size));
				if (new_data == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_OK;

				memcpy(new_data, data, a_Size);

				// Make sure the new pos is the current pos.
				m_CurrentPos = a_StartPos;

				// Make sure we add the size of this read buffer to the total size, so that on the next frame we will get the next part of the wave file.
				m_CurrentPos += a_Size;

				m_LastPlayedData = new_data;
				m_LastDataSize = a_Size;
				total_buffer_size += m_LastDataSize;

				AddEffects(new_data, a_Size);
				PlayBuffer(new_data, a_Size);
				m_DataBuffers.push({ new_data, a_Size });

				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}

			/// <summary>
			/// Plays a sound buffer.
			/// </summary>
			/// <param name="a_DataBuffer">The sound buffer.</param>
			/// <param name="a_Size">The sound buffer size.</param>
			UAUDIO_PLAYER_RESULT XAudio2Channel::PlayBuffer(const unsigned char* a_DataBuffer, uint32_t a_Size) const
			{
				XAUDIO2_BUFFER x_buffer = { 0, 0, nullptr, 0, 0, 0, 0, 0, nullptr };
				x_buffer.AudioBytes = a_Size;		// Buffer containing audio data.
				x_buffer.pAudioData = a_DataBuffer; // Size of the audio buffer in bytes.

				if (HRESULT hr; FAILED(hr = m_SourceVoice->SubmitSourceBuffer(&x_buffer)))
				{
					const _com_error err(hr);
					const LPCTSTR errMsg = err.ErrorMessage();
					LOGF(logger::LOGSEVERITY_ERROR, "<XAudio2> Submitting data to XAudio2 Source Voice failed: 0x%08x: %ls.", hr, errMsg);
				}
				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}


			UAUDIO_PLAYER_RESULT XAudio2Channel::Update()
			{
				if (m_Sound == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

				if (m_SourceVoice == nullptr)
					return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_XAUDIO2_NO_SOURCEVOICE;

				if (!m_IsPlaying)
					return UAUDIO_PLAYER_RESULT::UAUDIO_OK;

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

				PlayRanged(m_CurrentPos, real_buffersize);

				return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
			}
		}
	}
}