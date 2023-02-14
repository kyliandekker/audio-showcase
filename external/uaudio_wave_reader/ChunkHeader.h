#pragma once

#include <cstdint>
#include <cstring>

#include "./Defines.h"

namespace uaudio
{
	namespace wave_reader
	{
#pragma pack(push, 1)

		/*
		 * WHAT IS THIS FILE?
		 * This is a wave chunk header. Every chunk in a wave file has an ID and size.
		 * These are used in the WaveFormat class, where it holds every loaded chunk
		 * in an array.
		 *
			* The chunk id is always 4 bytes long and needs to be unique. Capitalization matters.
			* The chunk size is different for every chunk and determines how many bytes are read after this chunk header.
		 */
		struct ChunkHeader
		{
			ChunkHeader() {};
			ChunkHeader(ChunkHeader* a_DataBuffer)
			{
				memcpy(chunk_id, a_DataBuffer->chunk_id, CHUNK_ID_SIZE);
				chunkSize = a_DataBuffer->chunkSize;
			}

			unsigned char chunk_id[wave_reader::CHUNK_ID_SIZE] = {};
			uint32_t chunkSize = 0;
		};
#pragma pack(pop)
	}
}
