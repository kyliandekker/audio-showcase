#pragma once

#include <cstdint>

#include "utils/utils.h"
#include "audio/player/Defines.h"

namespace uaudio
{
	namespace effects
	{
		template <class T>
		inline T ChangeByteVolume(T a_Value, float a_Volume)
		{
			float converted_value = static_cast<float>(a_Value);
			converted_value *= a_Volume;

			return static_cast<T>(converted_value);
		}

		template <class T>
		inline void ChangeVolume(unsigned char*& a_DataBuffer, uint32_t a_Size, float a_Volume, uint16_t, uint16_t)
		{
			// Clamp the volume to 0.0 min and 1.0 max.
			a_Volume = clamp(a_Volume, player::UAUDIO_MIN_VOLUME, player::UAUDIO_MAX_VOLUME);

			T* array_16 = reinterpret_cast<T*>(a_DataBuffer);
			for (uint32_t i = 0; i < (a_Size / sizeof(T)); i++)
				array_16[i] = ChangeByteVolume<T>(array_16[i], a_Volume);

			a_DataBuffer = reinterpret_cast<unsigned char*>(array_16);
		}

		template <class T>
		inline void ChangePanning(unsigned char*& a_DataBuffer, uint32_t a_Size, float a_Amount, uint16_t a_NumChannels)
		{
			if (a_NumChannels == 1)
				return;

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

			T* array_16 = reinterpret_cast<T*>(a_DataBuffer);
			for (uint32_t i = 0; i < (a_Size / sizeof(T)); i += a_NumChannels)
			{
				array_16[i] = ChangeByteVolume<T>(array_16[i], left);
				array_16[i + 1] = ChangeByteVolume<T>(array_16[i + 1], right);
			}

			a_DataBuffer = reinterpret_cast<unsigned char*>(array_16);
		}
	}
}