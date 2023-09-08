#include "audio/storage/Sound.h"
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>

namespace uaudio
{
	namespace storage
	{
		Sound::~Sound()
		{
			if (m_ChunkCollection)
			{
				free(m_ChunkCollection->data());
				delete m_ChunkCollection;
			}
			free(m_Samples);
		}

		void Sound::Read(uint32_t a_StartingPoint, uint32_t& a_ElementCount, unsigned char*& a_DataBuffer) const
		{
			uaudio::wave_reader::DATA_Chunk data_chunk;
			m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
			// NOTE: This part will reduce the size of the buffer array. It is necessary when reaching the end of the file if we want to loop it.
			if (a_StartingPoint + a_ElementCount >= data_chunk.chunkSize)
			{
				const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - data_chunk.chunkSize);
				a_ElementCount = new_size;
			}
			a_DataBuffer = reinterpret_cast<unsigned char*>(utils::add(data_chunk.data, a_StartingPoint));
		}
	}
}