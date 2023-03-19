#pragma once

#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "./UAUDIO_WAVE_READER_RESULT.h"
#include "./ChunkCollection.h"
#include "./ChunkFilter.h"
#include "./Defines.h"

namespace uaudio
{
	namespace wave_reader
	{
		/*
		 * This is the wave reader. It is responsible for loading the chunks of a wave file and creating a ChunkCollection.
		 * It is also responsible for saving wave files.
		 * NOTE: Needs to be a class to be friends with ChunkCollection.
		*/
		class WaveReader
		{
		public:
			static UAUDIO_WAVE_READER_RESULT FTell(const char* a_FilePath, size_t& a_Size, ChunkFilter a_Filter = ChunkFilter(), uint16_t a_BitsPerSample = WAVE_BITS_PER_SAMPLE_16);
			static UAUDIO_WAVE_READER_RESULT LoadWave(const char* a_FilePath, ChunkCollection& a_ChunkCollection, ChunkFilter a_Filter = ChunkFilter(), uint16_t a_BitsPerSample = WAVE_BITS_PER_SAMPLE_16);
			static UAUDIO_WAVE_READER_RESULT SaveWave(const char* a_FilePath, const ChunkCollection& a_ChunkCollection);
		private:
			static UAUDIO_WAVE_READER_RESULT OpenWave(const char* a_FilePath, FILE*& a_File, long& a_FileSize);
			static UAUDIO_WAVE_READER_RESULT LoadChunk(const char* a_FilePath, FILE* a_File, long& a_PreviousTell, char* a_PrevChunkID, char* a_ChunkID, uint32_t& a_ChunkSize);
			static UAUDIO_WAVE_READER_RESULT CalculateConversionSize(long& a_Size, long a_CurrentSize, uint16_t a_OriginalBitsPerSample, uint16_t a_NewBitsPerSample);
		};
	}
}