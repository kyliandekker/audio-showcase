#pragma once

#include <cstdint>
#include <immintrin.h>

#include "utils/utils.h"
#include "audio/player/Defines.h"
#include "audio/player/AudioSystem.h"
#include "utils/Logger.h"
#include <iostream>

namespace uaudio
{
	namespace effects
	{
		template <class T>
		inline void ChangeVolume(unsigned char*& a_DataBuffer, uint32_t a_Size, float a_Volume)
		{
			// Clamp the volume to 0.0 min and 1.0 max.
			a_Volume = clamp(a_Volume, player::UAUDIO_MIN_VOLUME, player::UAUDIO_MAX_VOLUME);

			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
			T* result = reinterpret_cast<T*>(a_DataBuffer);

#pragma region NONSIMD
			if (!uaudio::player::audioSystem.simd)
			{
				for (uint32_t i = 0; i < a_Size / sizeof(T); i++)
					result[i] = static_cast<T>(static_cast<float>(result[i]) * a_Volume);
			}
#pragma endregion NONSIMD
#pragma region SIMD
			else
			{
				uint32_t skip = 4;

				const uint32_t size = (a_Size / sizeof(T));
				const uint32_t todo = size / skip;
				const uint32_t leftOver = size % skip;

				uint32_t i = 0;

				const __m128 mult = _mm_set_ps1(a_Volume);
				for (; i < todo; i += skip)
				{
					const __m128 fSamples = _mm_set_ps(
						static_cast<float>(result[i + 3]),
						static_cast<float>(result[i + 2]),
						static_cast<float>(result[i + 1]),
						static_cast<float>(result[i])
					);
					const __m128 fSamplesMultiplied = _mm_mul_ps(fSamples, mult);
					float fTemp[4];
					_mm_store_ps(fTemp, fSamplesMultiplied);

					result[i] = static_cast<T>(fTemp[0]);
					result[i + 1] = static_cast<T>(fTemp[1]);
					result[i + 2] = static_cast<T>(fTemp[2]);
					result[i + 3] = static_cast<T>(fTemp[3]);
				}
				for (; i < leftOver; i++)
				{
					T* result = reinterpret_cast<T*>(a_DataBuffer);

					for (uint32_t i = 0; i < a_Size / sizeof(T); i++)
						result[i] = static_cast<T>(static_cast<float>(result[i]) * a_Volume);

					a_DataBuffer = reinterpret_cast<unsigned char*>(result);
				}
			}
#pragma endregion SIMD
			a_DataBuffer = reinterpret_cast<unsigned char*>(result);

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			LOGF(logger::LOGSEVERITY_INFO, "(Volume) Time difference = %llu [ns].", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());
		}

		template <class T>
		inline void ChangePanning(unsigned char*& a_DataBuffer, uint32_t a_Size, float a_Amount, uint16_t a_NumChannels)
		{
			if (a_NumChannels == 1)
				return;

			std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

			// Amount is a value from -1 to 1.
			a_Amount = clamp(a_Amount, player::UAUDIO_MIN_PANNING, player::UAUDIO_MAX_PANNING);

			// Set the values to 1.0 as default.
			float left = player::UAUDIO_MAX_VOLUME, right = player::UAUDIO_MAX_VOLUME;

			// If the slider is more to the left.
			if (a_Amount < 0)
			{
				right += a_Amount;
				// Clamp the volume to 0.0 min and 1.0 max.
				right = clamp(right, player::UAUDIO_MIN_VOLUME, player::UAUDIO_MAX_VOLUME);
			}
			// If the slider is more to the right.
			else if (a_Amount > 0)
			{
				left -= a_Amount;
				// Clamp the volume to 0.0 min and 1.0 max.
				left = clamp(left, player::UAUDIO_MIN_VOLUME, player::UAUDIO_MAX_VOLUME);
			}

			T* result = reinterpret_cast<T*>(a_DataBuffer);

#pragma region NONSIMD
			if (!uaudio::player::audioSystem.simd)
			{
				for (uint32_t i = 0; i < (a_Size / sizeof(T)); i += 2)
				{
					result[i] = static_cast<T>(static_cast<float>(result[i]) * left);
					result[i + 1] = static_cast<T>(static_cast<float>(result[i + 1]) * right);
				}
			}
#pragma endregion NONSIMD
#pragma region SIMD
			else
			{
				uint32_t skip = 4;

				const uint32_t size = (a_Size / sizeof(T));
				const uint32_t todo = size / skip;
				const uint32_t leftOver = size % skip;

				uint32_t i = 0;

				const __m128 mult = _mm_set_ps(
					right,
					left,
					right,
					left
				);
				for (; i < todo; i += skip)
				{
					const __m128 fSamples = _mm_set_ps(
						static_cast<float>(result[i + 3]),
						static_cast<float>(result[i + 2]),
						static_cast<float>(result[i + 1]),
						static_cast<float>(result[i])
					);
					const __m128 fSamplesMultiplied = _mm_mul_ps(fSamples, mult);
					float fTemp[4];
					_mm_store_ps(fTemp, fSamplesMultiplied);

					result[i] = static_cast<T>(fTemp[0]);
					result[i + 1] = static_cast<T>(fTemp[1]);
					result[i + 2] = static_cast<T>(fTemp[2]);
					result[i + 3] = static_cast<T>(fTemp[3]);
				}
				for (;i < (a_Size / sizeof(T)); i += 2)
				{
					result[i] = static_cast<T>(static_cast<float>(result[i]) * left);
					result[i + 1] = static_cast<T>(static_cast<float>(result[i + 1]) * right);
				}
			}
#pragma endregion SIMD

			a_DataBuffer = reinterpret_cast<unsigned char*>(result);

			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			LOGF(logger::LOGSEVERITY_INFO, "(Panning) Time difference = %llu [ns].", std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count());
		}
	}
}