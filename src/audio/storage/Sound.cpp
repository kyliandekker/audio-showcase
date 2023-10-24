#include "audio/storage/Sound.h"

#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "utils/Logger.h"

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
			free(m_LeftSamples);
			free(m_RightSamples);
		}

		void Sound::Read(uint32_t a_StartingPoint, uint32_t& a_ElementCount, unsigned char*& a_DataBuffer) const
		{
			uaudio::wave_reader::DATA_Chunk data_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
			if (UAUDIOWAVEREADERFAILED(result))
			{
				LOGF(logger::LOGSEVERITY_WARNING, "Tried to read from sound %s, but it has no data chunk.", m_Name.c_str());
				a_DataBuffer = nullptr;
				return;
			}
			// NOTE: This part will reduce the size of the buffer array. It is necessary when reaching the end of the file if we want to loop it.
			if (a_StartingPoint + a_ElementCount >= data_chunk.ChunkSize())
			{
				const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - data_chunk.ChunkSize());
				a_ElementCount = new_size;
			}
			a_DataBuffer = reinterpret_cast<unsigned char*>(utils::add(data_chunk.data, a_StartingPoint));
		}

		void Sound::PreRead(uint32_t a_StartingPoint, uint32_t& a_ElementCount) const
		{
			uaudio::wave_reader::DATA_Chunk data_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
			if (UAUDIOWAVEREADERFAILED(result))
			{
				LOGF(logger::LOGSEVERITY_WARNING, "Tried to read from sound %s, but it has no data chunk.", m_Name.c_str());
				return;
			}
			// NOTE: This part will reduce the size of the buffer array. It is necessary when reaching the end of the file if we want to loop it.
			if (a_StartingPoint + a_ElementCount >= data_chunk.ChunkSize())
			{
				const uint32_t new_size = a_ElementCount - ((a_StartingPoint + a_ElementCount) - data_chunk.ChunkSize());
				a_ElementCount = new_size;
			}
		}
	}
}