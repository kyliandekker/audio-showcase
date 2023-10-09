#pragma once

#include "./UAUDIO_WAVE_READER_RESULT.h"
#include "./ChunkFilter.h"

namespace uaudio
{
	namespace wave_reader
	{
		class ChunkCollection;

		/*
		 * This is the wave reader. It is responsible for loading the chunks of a wave file and creating a ChunkCollection.
		 * It is also responsible for saving wave files.
		*/
		class WaveReader
		{
		public:
			static UAUDIO_WAVE_READER_RESULT FTell(const char* a_FilePath, size_t& a_Size, ChunkFilter a_Filter = ChunkFilter());
			static UAUDIO_WAVE_READER_RESULT LoadWave(const char* a_FilePath, ChunkCollection& a_ChunkCollection, ChunkFilter a_Filter = ChunkFilter());
			static UAUDIO_WAVE_READER_RESULT SaveWave(const char* a_FilePath, const ChunkCollection& a_ChunkCollection);
		private:
			static UAUDIO_WAVE_READER_RESULT OpenWave(const char* a_FilePath, FILE*& a_File, long& a_FileSize);
			static UAUDIO_WAVE_READER_RESULT LoadChunk(FILE* a_File, long& a_PreviousTell, char* a_PrevChunkID, char* a_ChunkID, uint32_t& a_ChunkSize);
		};
	}
}