#include "audio/player/AudioChannel.h"

#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>

#include "audio/storage/Sound.h"
#include "audio/player/utils.h"
#include "audio/player/Effects.h"
#include "audio/player/AudioSystem.h"

namespace uaudio
{
	namespace player
	{
		UAUDIO_PLAYER_RESULT AudioChannel::SetVolume(float a_Volume)
		{
			m_Volume = a_Volume;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::GetVolume(float& a_Volume) const
		{
			a_Volume = m_Volume;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::SetPanning(float a_Panning)
		{
			m_Panning = a_Panning;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::GetPanning(float& a_Panning) const
		{
			a_Panning = m_Panning;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::IsPlaying(bool& a_IsPlaying) const
		{
			a_IsPlaying = m_IsPlaying;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::SetActive(bool a_Active)
		{
			m_Active = a_Active;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::IsActive(bool& a_Active) const
		{
			a_Active = m_Active;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::IsInUse(bool& a_IsInUse) const
		{
			a_IsInUse = m_Sound != nullptr;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::SetSound(storage::Sound& a_Sound)
		{
			m_Sound = &a_Sound;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::GetSound(storage::Sound*& a_Sound)
		{
			if (m_Sound == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

			a_Sound = m_Sound;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::RemoveSound()
		{
			Stop();
			m_Sound = nullptr;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::SetLooping(bool a_Looping)
		{
			m_Looping = a_Looping;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::IsLooping(bool& a_Looping) const
		{
			a_Looping = m_Looping;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}
		UAUDIO_PLAYER_RESULT AudioChannel::Play()
		{
			m_IsPlaying = true;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::Pause()
		{
			m_IsPlaying = false;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::Stop()
		{
			Pause();
			m_CurrentPos = 0;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::Update()
		{
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::SetPos(uint32_t a_Pos)
		{
			m_CurrentPos = a_Pos;
			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::GetPos(TIMEUNIT a_TimeUnit, float& a_Pos) const
		{
			a_Pos = 0.0f;
			if (m_Sound == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_Sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
			if (result != uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;

			switch (a_TimeUnit)
			{
				case TIMEUNIT::TIMEUNIT_MS:
				{
					a_Pos = utils::PosToMilliseconds(m_CurrentPos, fmt_chunk.byteRate);
					break;
				}
				case TIMEUNIT::TIMEUNIT_S:
				{
					a_Pos = utils::PosToSeconds(m_CurrentPos, fmt_chunk.byteRate);
					break;
				}
				case TIMEUNIT::TIMEUNIT_POS:
				{
					a_Pos = static_cast<float>(m_CurrentPos);
					break;
				}
			}

			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		UAUDIO_PLAYER_RESULT AudioChannel::GetSoundBufferSize(uint32_t& a_BufferSize) const
		{
			a_BufferSize = 0;

			if (m_Sound == nullptr)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_SOUND_NOT_SET;

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_Sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
			if (result != uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
				return UAUDIO_PLAYER_RESULT::UAUDIO_ERR_NO_FMT_CHUNK;
			a_BufferSize = fmt_chunk.bitsPerSample / 8;

			return UAUDIO_PLAYER_RESULT::UAUDIO_OK;
		}

		void AudioChannel::AddEffects(unsigned char* a_Data, uint32_t a_BufferSize)
		{
			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_Sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
			if (result == uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT::UAUDIO_OK)
			{
				float masterVolume = 1.0f;
				uaudio::player::audioSystem.GetVolume(masterVolume);
				effects::ChangeVolume<int16_t>(a_Data, a_BufferSize, m_Volume * masterVolume, fmt_chunk.blockAlign, fmt_chunk.numChannels);

				float masterPanning = 0.0f;
				uaudio::player::audioSystem.GetPanning(masterPanning);
				effects::ChangePanning<int16_t>(a_Data, a_BufferSize, m_Panning * masterPanning, fmt_chunk.numChannels);
			}
		}
	}
}