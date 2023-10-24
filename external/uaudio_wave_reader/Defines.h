#pragma once

#include <cstdint>

namespace uaudio
{
	namespace wave_reader
	{
		constexpr uint32_t CHUNK_ID_SIZE = 4;

		// MONO/STEREO.
		constexpr uint16_t WAVE_CHANNELS_MONO = 1;
		constexpr uint16_t WAVE_CHANNELS_STEREO = 2;

		// BLOCK ALIGN SETTINGS
		constexpr uint16_t BLOCK_ALIGN_8_BIT_MONO = 1;
		constexpr uint16_t BLOCK_ALIGN_16_BIT_MONO = 2;
		constexpr uint16_t BLOCK_ALIGN_24_BIT_MONO = 3;
		constexpr uint16_t BLOCK_ALIGN_32_BIT_MONO = 4;
		constexpr uint16_t BLOCK_ALIGN_8_BIT_STEREO = BLOCK_ALIGN_8_BIT_MONO * WAVE_CHANNELS_STEREO;
		constexpr uint16_t BLOCK_ALIGN_16_BIT_STEREO = BLOCK_ALIGN_16_BIT_MONO * WAVE_CHANNELS_STEREO;
		constexpr uint16_t BLOCK_ALIGN_24_BIT_STEREO = BLOCK_ALIGN_24_BIT_MONO * WAVE_CHANNELS_STEREO;
		constexpr uint16_t BLOCK_ALIGN_32_BIT_STEREO = BLOCK_ALIGN_32_BIT_MONO * WAVE_CHANNELS_STEREO;

		// BITS_PER_SAMPLE SETTINGS
		constexpr uint16_t WAVE_BITS_PER_SAMPLE_8 = 8;
		constexpr uint16_t WAVE_BITS_PER_SAMPLE_16 = 16;
		constexpr uint16_t WAVE_BITS_PER_SAMPLE_24 = 24;
		constexpr uint16_t WAVE_BITS_PER_SAMPLE_32 = 32;
		constexpr uint16_t WAVE_BITS_PER_SAMPLE_32_FLOAT = 32;
		constexpr uint16_t WAVE_BITS_PER_SAMPLE_64 = 64;

		// SAMPLE RATE SETTINGS
		constexpr uint32_t WAVE_SAMPLE_RATE_11025 = 11025;
		constexpr uint32_t WAVE_SAMPLE_RATE_22050 = 22050;
		constexpr uint32_t WAVE_SAMPLE_RATE_44100 = 44100;
		constexpr uint32_t WAVE_SAMPLE_RATE_48000 = 48000;
		constexpr uint32_t WAVE_SAMPLE_RATE_88200 = 88200;
		constexpr uint32_t WAVE_SAMPLE_RATE_192000 = 192000;

		// http://www.bass.radio42.com/help/html/56c44e65-9b99-fa0d-d74a-3d9de3b01e89.htm

		// WAV FORMATS.
		constexpr int16_t WAV_FORMAT_UNKNOWN = 0;
		constexpr int16_t WAV_FORMAT_PCM = 1;
		constexpr int16_t WAV_FORMAT_UNCOMPRESSED = 1;
		constexpr int16_t WAV_FORMAT_MICROSOFT_ADPCM = 2;
		constexpr int16_t WAV_FORMAT_IEEE_FLOAT = 3;
		constexpr int16_t WAV_FORMAT_ITU_G711_ALAW = 6;
		constexpr int16_t WAV_FORMAT_ITU_G711_ÂΜLAW = 7;
		constexpr int16_t WAV_FORMAT_IMA_ADPCM = 17;
		constexpr int16_t WAV_FORMAT_ITU_G723_ADPCM = 20;
		constexpr int16_t WAV_FORMAT_ITU_G723_ADPCM_YAMAHA = 20;
		constexpr int16_t WAV_FORMAT_YAMAHA = 20;
		constexpr int16_t WAV_FORMAT_DOLBY = 48;
		constexpr int16_t WAV_FORMAT_GSM610 = 49;
		constexpr int16_t WAV_FORMAT_MSNAUDIO = 50;
		constexpr int16_t WAV_FORMAT_ITU_G721_ADPCM = 64;
		constexpr int16_t WAV_FORMAT_MPEG = 80;
		constexpr int16_t WAV_FORMAT_MPEGLAYER3 = 85;
		constexpr int16_t WAV_FORMAT_DOLBY_AC3_SPDIF = 146;
		constexpr int16_t WAV_FORMAT_RAW_AAC1 = 255;
		constexpr int16_t WAV_FORMAT_MSAUDIO1 = 352;
		constexpr int16_t WAV_FORMAT_WMA = 353;
		constexpr int16_t WAV_FORMAT_WMA_PRO = 354;
		constexpr int16_t WAV_FORMAT_WMA_LOSSLESS = 355;
		constexpr int16_t WAV_FORMAT_WMA_SPDIF = 356;
		constexpr int16_t WAV_FORMAT_MPEG_ADTS_AAC = 5632;
		constexpr int16_t WAV_FORMAT_MPEG_RAW_AAC = 5633;
		constexpr int16_t WAV_FORMAT_MPEG_LOAS = 5634;
		constexpr int16_t WAV_FORMAT_HEAAC = 5648;
		constexpr int16_t WAV_FORMAT_EXTENSIBLE = -2;
	}
}
