#pragma once

namespace uaudio
{
	namespace utils
	{
		/// <summary>
		/// Adds a specified size to a pointer.
		/// </summary>
		/// <param name="a_Ptr">The pointer</param>
		/// <param name="a_Size">The size that needs to be added.</param>
		/// <returns></returns>
		inline void* add(void* a_Ptr, size_t a_Size)
		{
			return reinterpret_cast<unsigned char*>(a_Ptr) + a_Size;
		}

		/// <summary>
		/// Function that reverses bytes.
		/// </summary>
		/// <param name="start">Start of the array of characters.</param>
		/// <param name="chunkID2">size of the array of characters.</param>
		/// <returns></returns>
		void reverseBytes(unsigned char* start, int size);

		/// <summary>
		/// Compares a chunk id to another chunk id.
		/// </summary>
		/// <param name="chunkID1">Chunk ID 1.</param>
		/// <param name="chunkID2">Chunk ID 2</param>
		/// <returns></returns>
		int chunkcmp(unsigned char* chunkID1, const char* chunkID2);

		/// <summary>
		/// Compares a chunk id to another chunk id.
		/// </summary>
		/// <param name="chunkID1">Chunk ID 1.</param>
		/// <param name="chunkID2">Chunk ID 2</param>
		/// <returns></returns>
		int chunkcmp(unsigned char* chunkID1, unsigned char* chunkID2);

		/// <summary>
		/// Function that reverses bytes and casts to specified type.
		/// </summary>
		/// <param name="number">The data that needs to be casted.</param>
		/// <returns></returns>
		template <class T>
		inline T reverseBytesC(unsigned char* number)
		{
			reverseBytes(number, sizeof(T));
			return *reinterpret_cast<T*>(number);
		}
	}
}