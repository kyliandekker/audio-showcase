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
	}
}