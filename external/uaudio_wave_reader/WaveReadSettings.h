#pragma once

#include "UAUDIO_WAVE_READER_RESULT.h"

namespace uaudio
{
	namespace wave_reader
	{
		enum class BPSConversionSettings
		{
			CONVERSION_IDC,
			CONVERSION_8_BIT,
			CONVERSION_16_BIT,
			CONVERSION_24_BIT,
			CONVERSION_32_BIT
		};

		enum class ChannelsConversionSettings
		{
			CONVERSION_IDC,
			CONVERSION_MONO,
			CONVERSION_STEREO
		};

		class ChunkFilter;

		class WaveReadSettings
		{
		public:
			WaveReadSettings();
			WaveReadSettings(ChunkFilter& a_ChunkFilter);

			UAUDIO_WAVE_READER_RESULT SetChunkFilter(ChunkFilter& a_ChunkFilter);
			UAUDIO_WAVE_READER_RESULT Channels(ChannelsConversionSettings& a_Channels) const;
			UAUDIO_WAVE_READER_RESULT BitsPerSample(BPSConversionSettings& a_BitsPerSample) const;

			UAUDIO_WAVE_READER_RESULT SetChannels(ChannelsConversionSettings a_Channels);
			UAUDIO_WAVE_READER_RESULT SetBitsPerSample(BPSConversionSettings a_BitsPerSample);

			friend class WaveReader;
		private:
			ChunkFilter* m_ChunkFilter = nullptr;
			ChannelsConversionSettings m_Channels = ChannelsConversionSettings::CONVERSION_IDC;
			BPSConversionSettings m_BitsPerSample = BPSConversionSettings::CONVERSION_IDC;
		};
	}
}