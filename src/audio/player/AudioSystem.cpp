#include "audio/player/AudioSystem.h"

#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/ChunkCollection.h>

#include "audio/player/Sound.h"

uaudio::player::AudioSystem uaudio::player::audioSystem;

namespace uaudio
{
	namespace player
	{
		void AudioSystem::AddSound(const char* a_Path)
		{
			uaudio::player::Hash hash = uaudio::player::GetHash(a_Path);
			if (m_Sounds.find(hash) != m_Sounds.end())
				return;

			size_t size = 0;
			uaudio::wave_reader::WaveReader::FTell(a_Path, size);
			void* allocated_space = malloc(size);

			uaudio::wave_reader::ChunkCollection* chunkCollection = new uaudio::wave_reader::ChunkCollection(allocated_space, size);
			uaudio::wave_reader::WaveReader::LoadWave(a_Path, *chunkCollection);

			Sound* sound = new Sound();
			sound->m_ChunkCollection = chunkCollection;
			sound->m_Hash = hash;
			sound->m_Name = a_Path;

			m_Sounds.insert(std::make_pair(hash, sound));
		}

		void AudioSystem::UnloadSound(uaudio::player::Hash a_Hash)
		{
			delete m_Sounds[a_Hash];
			m_Sounds.erase(a_Hash);
		}

        std::vector<Sound*> AudioSystem::GetSounds() const
        {
			std::vector<Sound*> sounds;
			for (auto& resource : m_Sounds)
				sounds.push_back(resource.second);
            return sounds;
        }
	}
}