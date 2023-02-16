#include "audio/player/SoundsSystem.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "audio/player/Sound.h"
#include "audio/utils/Utils.h"

uaudio::player::SoundsSystem uaudio::player::soundSystem;

namespace uaudio
{
	namespace player
	{
		void SoundsSystem::AddSound(const char* a_Path, uaudio::wave_reader::ChunkFilter a_Filter)
		{
			uaudio::player::Hash hash = uaudio::player::GetHash(a_Path);
			if (m_Sounds.find(hash) != m_Sounds.end())
				return;

			size_t size = 0;
			uaudio::wave_reader::WaveReader::FTell(a_Path, size, a_Filter);
			void* allocated_space = malloc(size);

			uaudio::wave_reader::ChunkCollection* chunkCollection = new uaudio::wave_reader::ChunkCollection(allocated_space, size);
			uaudio::wave_reader::WaveReader::LoadWave(a_Path, *chunkCollection, a_Filter);

			Sound* sound = new Sound();
			sound->m_ChunkCollection = chunkCollection;
			sound->m_Hash = hash;
			sound->m_Name = a_Path;

			bool hasDataChunk = false;
			chunkCollection->HasChunk(hasDataChunk, uaudio::wave_reader::DATA_CHUNK_ID);

			if (hasDataChunk)
			{
				uaudio::wave_reader::DATA_Chunk data_chunk;
				chunkCollection->GetChunkFromData<uaudio::wave_reader::DATA_Chunk>(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
				uint32_t data_chunk_size = 0;
				chunkCollection->GetChunkSize(data_chunk_size, uaudio::wave_reader::DATA_CHUNK_ID);
				float* samples = uaudio::utils::ToSample(data_chunk.data, data_chunk_size);
				sound->m_Samples = samples;
			}

			m_Sounds.insert(std::make_pair(hash, sound));
		}

		void SoundsSystem::UnloadSound(uaudio::player::Hash a_Hash)
		{
			delete m_Sounds[a_Hash];
			m_Sounds.erase(a_Hash);
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