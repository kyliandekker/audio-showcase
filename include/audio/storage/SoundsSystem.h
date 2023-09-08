#pragma once

#include <unordered_map>
#include <uaudio_wave_reader/ChunkFilter.h>

#include "utils/Hash.h"
#include "audio/storage/Sound.h"

namespace uaudio
{
	namespace storage
	{
		class SoundsSystem
		{
		public:
			Sound* AddSound(const char* a_Path, uaudio::wave_reader::ChunkFilter a_Filter);
			void UnloadSound(uaudio::player::Hash a_Hash);
			std::vector<Sound*> GetSounds() const;
		private:
			std::unordered_map<uaudio::player::Hash, Sound*> m_Sounds;
		};
		extern SoundsSystem soundSystem;
	}
}