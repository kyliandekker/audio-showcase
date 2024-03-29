#pragma once

#include <cstdint>
#include <string>

namespace uaudio
{
	namespace player
	{
		namespace utils
		{
			float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate);
			float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate);
			uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate);
			uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate);
			float SecondsToMilliseconds(float m_Seconds);
			float MillisecondsToSeconds(float m_Milliseconds);
			uint32_t SecondsToHours(float m_Seconds);
			uint32_t SecondsToMinutes(float m_Seconds);
			float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);
			std::string FormatDuration(float a_Duration, bool a_Milliseconds);
			inline unsigned char* add(void* a_Ptr, size_t a_Size)
			{
				return reinterpret_cast<unsigned char*>(a_Ptr) + a_Size;
			}

			bool chunkcmp(const char* a_ChunkID1, const char* a_ChunkID2);
			double* ToSample(unsigned char* data, size_t buffersize, uint16_t bitsPerSample, uint16_t blockAlign, uint16_t channels, uint16_t audioFormat, size_t numSamples, bool left = true);
			double GetPeak(unsigned char* data, size_t data_size, uint16_t bitsPerSample, uint16_t blockAlign, uint16_t channels, uint16_t audioFormat, size_t scale, bool left = true);
		}
	}
}