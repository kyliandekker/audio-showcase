#pragma once

#include <unordered_map>
#include <uaudio_wave_reader/WaveReader.h>

#include "utils/Hash.h"

namespace uaudio
{
	namespace player
	{
		class Sound;

		class SoundsSystem
		{
		public:
			void AddSound(const char* a_Path, uaudio::wave_reader::Filter a_Filter);
			void UnloadSound(uaudio::player::Hash a_Hash);
			std::vector<Sound*> GetSounds() const;
		private:
			std::unordered_map<uaudio::player::Hash, uaudio::player::Sound*> m_Sounds;
		};
		extern SoundsSystem soundSystem;
	}
}