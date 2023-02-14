#pragma once

#include <cassert>

#include "./Utils.h"
#include "./ChunkHeader.h"
#include "./UAUDIO_WAVE_READER_RESULT.h"

namespace uaudio
{
	namespace wave_reader
	{
		/*
		 * WHAT IS THIS FILE?
		 * This is the ChunkCollection class, where all the chunks are stored.
		 * It has methods to retrieve specific chunks and can remove chunks as well.
		 *
		 * This class does not hold information such as volume, panning, looping and pitch. It only stored the actual wave file information.
		*/

		class WaveReader;

		class ChunkCollection
		{
		private:
			void* m_Start = nullptr;
			void* m_Buffer = nullptr;
			void* m_End = nullptr;
			size_t m_Size = 0;

			friend class WaveReader;

			/// <summary>
			/// Retrieves a chunk with a specific chunk id from the loaded chunks.
			/// </summary>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <returns>The chunk on success, nullptr on failure.</returns>
			ChunkHeader* GetChunkFromAllocator(const char* a_ChunkID) const
			{
				assert(m_Start != nullptr);

				unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
				while (data < m_End)
				{
					ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(data);
					if (strncmp(&a_ChunkID[0], &reinterpret_cast<char*>(wave_chunk_data->chunk_id)[0], CHUNK_ID_SIZE) == 0)
					{
						return wave_chunk_data;
					}
					data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(ChunkHeader) + wave_chunk_data->chunkSize));
				}
				return nullptr;
			}

			/// <summary>
			/// Retrieves a chunk by index.
			/// </summary>
			/// <param name="a_Index">The index.</param>
			/// <returns>The chunk on success, nullptr on failure.</returns>
			ChunkHeader* GetChunkFromAllocator(uint32_t a_Index) const
			{
				assert(m_Start != nullptr);

				unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
				uint32_t index = 0;
				while (data < m_End)
				{
					ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(data);
					data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(ChunkHeader) + wave_chunk_data->chunkSize));
					if (index == a_Index)
						return wave_chunk_data;
					index++;
				}
				return nullptr;
			}

			uint32_t GetNumberOfChunks() const
			{
				assert(m_Start != nullptr);

				unsigned char* data = reinterpret_cast<unsigned char*>(m_Start);
				uint32_t index = 0;
				while (data < m_End)
				{
					ChunkHeader* wave_chunk_data = reinterpret_cast<ChunkHeader*>(data);
					data = reinterpret_cast<unsigned char*>(utils::add(data, sizeof(ChunkHeader) + wave_chunk_data->chunkSize));
					index++;
				}
				return index;
			}

			void* Alloc(size_t a_Size)
			{
				assert(m_Start != nullptr);
				assert(m_Size > 0);
				void* current = m_Buffer;
				m_Buffer = utils::add(m_Buffer, a_Size);
				return current;
			}

			void* Start() const
			{
				return m_Start;
			}

			void* End() const
			{
				return m_End;
			}

			void* GetBuffer() const
			{
				return m_Buffer;
			}
		public:

			ChunkCollection(void* a_Ptr, size_t a_Size)
			{
				assert(a_Size != 0);
				m_Size = a_Size;
				m_Start = a_Ptr;
				m_Buffer = m_Start;
				m_End = utils::add(m_Start, m_Size);
			}

			ChunkCollection(const ChunkCollection& rhs) = default;

			~ChunkCollection() = default;

			ChunkCollection& operator=(const ChunkCollection& rhs) = default;

			/// <summary>
			/// Returns the data.
			/// </summary>
			/// <returns></returns>
			void* GetData() const
			{
				return m_Start;
			}

			/// <summary>
			/// Adds a chunk to the chunk list.
			/// </summary>
			/// <param name="a_ChunkData"></param>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <param name="a_Chunk">The chunk data.</param>
			template <class T>
			static UAUDIO_WAVE_READER_RESULT ConstructChunk(ChunkHeader*& a_ChunkData, const char* a_ChunkID, T& a_Chunk)
			{
				// Set the chunk id.
				if (a_ChunkID != nullptr)
					memcpy(a_ChunkData->chunk_id, a_ChunkID, CHUNK_ID_SIZE);

				// Set the chunk size.
				a_ChunkData->chunkSize = sizeof(T);

				T* custom_chunk = reinterpret_cast<T*>(utils::add(a_ChunkData, sizeof(ChunkHeader)));
				memcpy(custom_chunk, &a_Chunk, sizeof(T));

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
			}

			/// <summary>
			/// Returns the first chunk that shares the chunk id.
			/// </summary>
			template <class T>
			UAUDIO_WAVE_READER_RESULT GetChunkFromData(T& a_Type, const char* a_ChunkID) const
			{
				ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
				if (data != nullptr)
				{
					a_Type = T(reinterpret_cast<T*>(utils::add(data, 0)));
					return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
				}

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Returns a chunk by index.
			/// </summary>
			template <class T>
			UAUDIO_WAVE_READER_RESULT GetChunkFromData(T& a_Type, uint32_t a_Index) const
			{
				ChunkHeader* data = GetChunkFromAllocator(a_Index);
				if (data != nullptr)
				{
					a_Type = T(reinterpret_cast<T*>(utils::add(data, 0)));
					return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
				}

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Gets the chunk size of the first chunk that shares the chunk id.
			/// </summary>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <returns>UAudio Result.</returns>
			UAUDIO_WAVE_READER_RESULT GetChunkSize(uint32_t& a_Size, const char* a_ChunkID) const
			{
				a_Size = 0;
				ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
				if (data != nullptr)
				{
					a_Size = data->chunkSize;
					return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
				}

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Returns the number of loaded chunks.
			/// </summary>
			/// <param name="a_Size"></param>
			/// <returns>UAudio Result.</returns>
			UAUDIO_WAVE_READER_RESULT GetNumberOfChunks(size_t& a_Size) const
			{
				a_Size = GetNumberOfChunks();
				return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
			}

			/// <summary>
			/// Checks whether a chunk that shares the chunk id exists.
			/// </summary>
			/// <param name="a_ChunkID">The chunk id (must be a length of 4 characters).</param>
			/// <returns>UAudio Result.</returns>
			UAUDIO_WAVE_READER_RESULT HasChunk(bool& a_ChunkFound, const char* a_ChunkID) const
			{
				a_ChunkFound = false;

				ChunkHeader* data = GetChunkFromAllocator(a_ChunkID);
				if (data != nullptr)
				{
					a_ChunkFound = true;
					return UAUDIO_WAVE_READER_RESULT::UAUDIO_OK;
				}

				return UAUDIO_WAVE_READER_RESULT::UAUDIO_ERR_CHUNK_NOT_FOUND;
			}

			/// <summary>
			/// Returns the total size of all chunks.
			/// </summary>
			/// <returns></returns>
			size_t GetSize() const
			{
				return m_Size;
			}
		};
	}
}
