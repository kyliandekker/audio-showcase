#pragma once

#include <vector>
#include <uaudio_wave_reader/WaveReadSettings.h>

#include "utils/Hash.h"
#include "audio/storage/Sound.h"

namespace uaudio
{
	namespace storage
	{
		class SoundsSystem
		{
		public:
			Sound* AddSound(const char* a_Path, const uaudio::wave_reader::WaveReadSettings& a_Settings = uaudio::wave_reader::WaveReadSettings());
			void UnloadSound(uaudio::player::Hash a_Hash);
			const std::vector<Sound*>& GetSounds() const;
		private:
			std::vector<Sound*> m_Sounds;
		};
		extern SoundsSystem soundSystem;
	}
}