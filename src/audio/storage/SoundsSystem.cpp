#include "audio/storage/SoundsSystem.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "audio/storage/Sound.h"
#include "audio/player/utils.h"
#include "utils/Logger.h"

uaudio::storage::SoundsSystem uaudio::storage::soundSystem;

namespace uaudio
{
	namespace storage
	{
		constexpr int MAX_SAMPLES = 70560;

		Sound* SoundsSystem::AddSound(const char* a_Path, const uaudio::wave_reader::WaveReadSettings& a_Settings)
		{
			uaudio::player::Hash hash = uaudio::player::GetHash(a_Path);
			for (size_t i = 0; i < m_Sounds.size(); i++)
			{
				if (m_Sounds[i]->m_Hash == hash)
					return nullptr;
			}

			size_t size = 0;
			uaudio::wave_reader::WaveReader::FTell(a_Path, size, a_Settings);

			if (size == 0)
			{
				LOG(logger::LOGSEVERITY_ERROR, "No chunks were found in the sound you tried to load.");
				return nullptr;
			}

			void* allocated_space = malloc(size);

			uaudio::wave_reader::ChunkCollection* chunkCollection = new uaudio::wave_reader::ChunkCollection(allocated_space, size);
			uaudio::wave_reader::WaveReader::LoadWave(a_Path, *chunkCollection, a_Settings);

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = chunkCollection->GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);

			Sound* sound = new Sound();
			sound->m_ChunkCollection = chunkCollection;
			sound->m_Hash = hash;
			sound->m_FullName = a_Path;
			std::string path = a_Path;
			sound->m_Name = path.substr(path.find_last_of("\\") + 1);
			
			if (!(UAUDIOWAVEREADERFAILED(result)))
			{
				if (fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_8 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_24 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_32 && fmt_chunk.bitsPerSample != uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_64)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Bits per sample not supported.");
					free(allocated_space);
					return nullptr;
				}
				if (fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8 != fmt_chunk.blockAlign)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Block align is wrong.");
					free(allocated_space);
					return nullptr;
				}
				if (fmt_chunk.sampleRate * fmt_chunk.numChannels * fmt_chunk.bitsPerSample / 8 != fmt_chunk.byteRate)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Byte rate is wrong.");
					free(allocated_space);
					return nullptr;
				}
				if (fmt_chunk.numChannels > uaudio::wave_reader::WAVE_CHANNELS_STEREO)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Too many channels.");
					free(allocated_space);
					return nullptr;
				}
				if (fmt_chunk.numChannels < uaudio::wave_reader::WAVE_CHANNELS_MONO)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Too few channels.");
					free(allocated_space);
					return nullptr;
				}
				if (fmt_chunk.audioFormat == uaudio::wave_reader::WAV_FORMAT_IEEE_FLOAT && fmt_chunk.bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_64)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Playing 64-float format is not supported. You can still view this file's chunks, however.");
				}
				if (fmt_chunk.audioFormat != uaudio::wave_reader::WAV_FORMAT_IEEE_FLOAT && fmt_chunk.audioFormat != uaudio::wave_reader::WAV_FORMAT_PCM)
				{
					LOG(logger::LOGSEVERITY_ERROR, "Unknown format. Playing is not supported. You can still view this file's chunks, however.");
				}
			}

			uaudio::wave_reader::DATA_Chunk data_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result2 = chunkCollection->GetChunkFromData<uaudio::wave_reader::DATA_Chunk>(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
			if (!(UAUDIOWAVEREADERFAILED(result)) && !(UAUDIOWAVEREADERFAILED(result2)))
			{
				uint32_t data_chunk_size = 0;
				chunkCollection->GetChunkSize(data_chunk_size, uaudio::wave_reader::DATA_CHUNK_ID);
				sound->m_RNumSamples = data_chunk_size / fmt_chunk.blockAlign;
				sound->m_NumSamples = sound->m_RNumSamples;
				if (sound->m_NumSamples > MAX_SAMPLES)
					sound->m_NumSamples = MAX_SAMPLES;
				sound->m_LeftSamples = uaudio::player::utils::ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, sound->m_NumSamples);
				sound->m_RightSamples = uaudio::player::utils::ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, sound->m_NumSamples, false);
			}

			m_Sounds.push_back(sound);

			LOGF(logger::LOGSEVERITY_INFO, "Loaded sound: %s.", sound->m_Name.c_str());

			return sound;
		}

		void SoundsSystem::UnloadSound(uaudio::player::Hash a_Hash)
		{
			Sound* sound = nullptr;
			for (size_t i = 0; i < m_Sounds.size(); i++)
			{
				if (m_Sounds[i]->m_Hash == a_Hash)
				{
					sound = m_Sounds[i];
					m_Sounds.erase(m_Sounds.begin() + i);
				}
			}
			if (sound)
				delete sound;
		}

        const std::vector<Sound*>& SoundsSystem::GetSounds() const
        {
			return m_Sounds;
        }
	}
}