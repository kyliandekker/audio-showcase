#pragma once

#include <array>
#include <string>
#include <vector>

#include <uaudio_wave_reader/Defines.h>

#include "audio/player/Defines.h"
#include "imgui/tools/BaseTool.h"

namespace uaudio
{
	namespace imgui
	{
		struct chunk_select
		{
			std::string chunk_id;
			bool selected = false;
			bool removable = true;
		};

		class MasterTool : public BaseTool
		{
		public:
			MasterTool();
			void Render() override;

		private:
			void OpenFile();

			std::array<const char*, 7> m_BufferSizeTextOptions = {
				"256",
				"384",
				"512",
				"1024",
				"2048",
				"4096",
				"8192",
			};

			uint32_t m_BufferSizeSelection = 0;

			std::array<uint32_t, 7> m_BufferSizeOptions = {
				256,
				384,
				512,
				1024,
				2048,
				4096,
				8192,
			};

			std::array<const char*, 4> m_BitsPerSampleTextOptions = {
				"I don't really care",
				"16-bit",
				"24-bit",
				"32-bit",
			};

			std::array<uint16_t, 4> m_BitsPerSampleOptions = {
				0,
				uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_16,
				uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_24,
				uaudio::wave_reader::WAVE_BITS_PER_SAMPLE_32,
			};

			std::array<const char*, 3> m_ChannelsTextOptions = {
				"I don't really care",
				"Mono",
				"Stereo"
			};

			std::array<const char*, 4> m_LoopPointTextOptions = {
				"None",
				"Load Start Point",
				"Load End Point",
				"Load Start & End Point"
			};

			std::vector<chunk_select> m_ChunkIds;
			char m_SelectedChunkName[uaudio::wave_reader::CHUNK_ID_SIZE] = 
			{
			};

			uint32_t m_SelectedBitsPerSample = 0;
		};
	}
}