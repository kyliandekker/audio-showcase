#pragma once

#include <string>

#include "utils/Hash.h"
#include <mutex>

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkCollection;
	}

	namespace storage
	{
		class Sound
		{
		public:
			~Sound();
			std::string m_Name;
			std::string m_FullName;
			uaudio::player::Hash m_Hash = HASH_INVALID;
			uaudio::wave_reader::ChunkCollection* m_ChunkCollection = nullptr;
			double* m_LeftSamples = nullptr;
			double* m_RightSamples = nullptr;
			size_t m_NumSamples = 0;
			size_t m_RNumSamples = 0;
			std::mutex m_Mutex;

			void Read(uint32_t a_StartingPoint, uint32_t& a_ElementCount, unsigned char*& a_DataBuffer) const;
			void PreRead(uint32_t a_StartingPoint, uint32_t& a_ElementCount) const;
		};
	}
}