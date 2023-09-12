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
		Sound* SoundsSystem::AddSound(const char* a_Path, uaudio::wave_reader::ChunkFilter a_Filter)
		{
			uaudio::player::Hash hash = uaudio::player::GetHash(a_Path);
			if (m_Sounds.find(hash) != m_Sounds.end())
				return nullptr;

			size_t size = 0;
			uaudio::wave_reader::WaveReader::FTell(a_Path, size, a_Filter);

			if (size == 0)
				return nullptr;

			void* allocated_space = malloc(size);

			uaudio::wave_reader::ChunkCollection* chunkCollection = new uaudio::wave_reader::ChunkCollection(allocated_space, size);
			uaudio::wave_reader::WaveReader::LoadWave(a_Path, *chunkCollection, a_Filter);

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			chunkCollection->GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
			if (fmt_chunk.bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_24)
			{
				LOG(logger::LOGSEVERITY_ERROR, "Cannot load 24-bit wave files yet.");
				free(allocated_space);
				return nullptr;
			}

			Sound* sound = new Sound();
			sound->m_ChunkCollection = chunkCollection;
			sound->m_Hash = hash;
			sound->m_FullName = a_Path;
			std::string path = a_Path;
			sound->m_Name = path.substr(path.find_last_of("\\") + 1);

			bool hasDataChunk = false;
			chunkCollection->HasChunk(hasDataChunk, uaudio::wave_reader::DATA_CHUNK_ID);

			if (hasDataChunk)
			{
				uaudio::wave_reader::DATA_Chunk data_chunk;
				chunkCollection->GetChunkFromData<uaudio::wave_reader::DATA_Chunk>(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
				uint32_t data_chunk_size = 0;
				chunkCollection->GetChunkSize(data_chunk_size, uaudio::wave_reader::DATA_CHUNK_ID);
				sound->m_Samples = uaudio::player::utils::ToSample(data_chunk.data, data_chunk_size, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels);
				sound->m_NumSamples = data_chunk_size / fmt_chunk.blockAlign;
			}

			m_Sounds.insert(std::make_pair(hash, sound));

			LOGF(logger::LOGSEVERITY_INFO, "Loaded sound: %s.", sound->m_Name.c_str());

			return sound;
		}

		void SoundsSystem::UnloadSound(uaudio::player::Hash a_Hash)
		{
			Sound* s = m_Sounds[a_Hash];
			m_Sounds.erase(a_Hash);
			delete s;
		}

        std::vector<Sound*> SoundsSystem::GetSounds() const
        {
			std::vector<Sound*> sounds;
			for (auto& resource : m_Sounds)
				sounds.push_back(resource.second);
            return sounds;
        }
	}
}