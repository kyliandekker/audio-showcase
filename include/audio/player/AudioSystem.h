#pragma once

#include <unordered_map>

#include "utils/Hash.h"

namespace uaudio
{
	namespace player
	{
		class Sound;

		class AudioSystem
		{
		public:
			void AddSound(const char* a_Path);
			void UnloadSound(uaudio::player::Hash a_Hash);
			std::vector<Sound*> GetSounds() const;
		private:
			std::unordered_map<uaudio::player::Hash, uaudio::player::Sound*> m_Sounds;
		};
		extern AudioSystem audioSystem;
	}
}