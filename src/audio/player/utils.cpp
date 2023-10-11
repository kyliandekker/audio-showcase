#include "audio/player/utils.h"

#include <uaudio_wave_reader/Defines.h>

namespace uaudio
{
	namespace player
	{
		namespace utils
		{
			float PosToMilliseconds(uint32_t m_Pos, uint32_t a_ByteRate)
			{
				return SecondsToMilliseconds(PosToSeconds(m_Pos, a_ByteRate));
			}

			float PosToSeconds(uint32_t m_Pos, uint32_t a_ByteRate)
			{
				if (a_ByteRate == 0)
					return 0;

				return static_cast<float>(m_Pos) / static_cast<float>(a_ByteRate);
			}

			uint32_t SecondsToPos(float m_Seconds, uint32_t a_ByteRate)
			{
				if (a_ByteRate == 0)
					return 0;

				return static_cast<uint32_t>(m_Seconds * static_cast<float>(a_ByteRate));
			}

			uint32_t MillisecondsToPos(float m_Milliseconds, uint32_t a_ByteRate)
			{
				if (a_ByteRate == 0)
					return 0;

				return SecondsToPos(MillisecondsToSeconds(m_Milliseconds), a_ByteRate);
			}

			float SecondsToMilliseconds(float m_Seconds)
			{
				return m_Seconds * 1000;
			}

			float MillisecondsToSeconds(float m_Milliseconds)
			{
				return m_Milliseconds / 1000;
			}

			uint32_t SecondsToHours(float m_Seconds)
			{
				return static_cast<uint32_t>(m_Seconds) / 3600;
			}

			uint32_t SecondsToMinutes(float m_Seconds)
			{
				return (static_cast<uint32_t>(m_Seconds) - (SecondsToHours(m_Seconds) * 3600)) / 60;
			}

			float GetDuration(uint32_t a_ChunkSize, uint32_t a_ByteRate)
			{
				return static_cast<float>(a_ChunkSize) / static_cast<float>(a_ByteRate);
			}

			std::string FormatDuration(float a_Duration, bool a_Milliseconds)
			{
				const uint32_t hours = utils::SecondsToHours(a_Duration);
				const uint32_t minutes = utils::SecondsToMinutes(a_Duration);
				const uint32_t seconds = static_cast<uint32_t>(a_Duration) % 60;
				const uint32_t total = (hours * 3600) + (minutes * 60) + seconds;
				const float milliseconds_float = a_Duration - static_cast<float>(total);
				const uint32_t milliseconds = static_cast<uint32_t>(milliseconds_float * 1000);

				char hours_string[32], minutes_string[32], seconds_string[32], milliseconds_string[32];
				sprintf_s(hours_string, "%02d", hours);
				sprintf_s(minutes_string, "%02d", minutes);
				sprintf_s(seconds_string, "%02d", seconds);
				sprintf_s(milliseconds_string, "%03d", milliseconds);
				return std::string(hours_string) +
					":" +
					std::string(minutes_string) +
					":" +
					std::string(seconds_string) + (a_Milliseconds ? ":" + std::string(milliseconds_string) : "");
			}

			bool chunkcmp(const char* a_ChunkID1, const char* a_ChunkID2)
			{
				return (strncmp(a_ChunkID1, a_ChunkID2, uaudio::wave_reader::CHUNK_ID_SIZE) == 0);
			}

			float* ToSample(unsigned char* data, size_t buffersize, uint16_t bitsPerSample, uint16_t blockAlign, uint16_t channels, size_t numSamples)
			{
				unsigned char* pData = data;
				size_t realNumSamples = buffersize / blockAlign;

				size_t div = realNumSamples / numSamples;

				float* samples = reinterpret_cast<float*>(malloc(numSamples * sizeof(float)));
				if (!samples)
					return nullptr;

				if (bitsPerSample == uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16 && channels == uaudio::wave_reader::WAVE_CHANNELS_STEREO)
				{
					for (size_t i = 0; i < numSamples; i++)
					{
						int16_t left = *(int16_t*)pData;
						pData += div * (sizeof(int16_t) + sizeof(int16_t));
						samples[i] = static_cast<float>(left) / 32768.0f;
					}
				}

				return samples;
			}
		}
	}
}