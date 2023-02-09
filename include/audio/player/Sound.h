#pragma once

#include <string>

#include "utils/Hash.h"

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkCollection;
	}

	namespace player
	{
		class Sound
		{
		public:
			~Sound();
			std::string m_Name;
			uaudio::player::Hash m_Hash = HASH_INVALID;
			uaudio::wave_reader::ChunkCollection* m_ChunkCollection = nullptr;
			float* m_Samples = nullptr;
		};
	}
}