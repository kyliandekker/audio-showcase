#pragma once

#include <cstdint>
#include <string>

namespace uaudio
{
	namespace utils
	{
		float PosToMilliseconds(uint32_t m_Position, uint32_t a_ByteRate);
		float PosToSeconds(uint32_t m_Position, uint32_t a_ByteRate);
		uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate);
		uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate);
		float SecondsToMilliseconds(float m_Seconds);
		float MillisecondsToSeconds(float m_Milliseconds);
		uint32_t SecondsToHours(float m_Seconds);
		uint32_t SecondsToMinutes(float m_Seconds);
		float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate);

		std::string FormatDuration(float a_Duration, bool a_Milliseconds = true);

		bool chunkcmp(const char* a_ChunkID1, const char* a_ChunkID2);
		float* ToSample(unsigned char* data, size_t buffersize);
	}
}