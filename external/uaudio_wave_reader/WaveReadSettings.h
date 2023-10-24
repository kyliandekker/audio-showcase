#pragma once

namespace uaudio
{
	namespace wave_reader
	{
		enum class UAUDIO_WAVE_READER_RESULT;

		enum class BPSConversionSettings
		{
			CONVERSION_IDC = 0,
			CONVERSION_8_BIT = 8,
			CONVERSION_16_BIT = 16,
			CONVERSION_24_BIT = 24,
			CONVERSION_32_BIT = 32
		};

		enum class ChannelsConversionSettings
		{
			CONVERSION_IDC = 0,
			CONVERSION_MONO = 1,
			CONVERSION_STEREO = 2
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