#pragma once

#include "imgui/tools/BaseTool.h"

namespace uaudio
{
	namespace storage
	{
		class Sound;
	}

	namespace wave_reader
	{
		class ChunkCollection;
		struct ChunkHeader;
	}

	namespace imgui
	{
		class SoundsTool : public BaseTool
		{
		public:
			SoundsTool();
			void Render() override;
            void ShowBaseChunk(char* a_ChunkId, uaudio::wave_reader::ChunkCollection& chunkCollection);
			bool RenderSound(uaudio::storage::Sound& a_Sound);
			void SaveFile(uaudio::wave_reader::ChunkCollection& chunkCollection);

		private:
            template<class T>
            void ViewAs(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness);
			void ViewAsChar(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness);
			void ViewAsString(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness);
        };
	}
}