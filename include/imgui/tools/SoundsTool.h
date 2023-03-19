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
			void RenderSound(uaudio::storage::Sound& a_Sound);
			void SaveFile(uaudio::wave_reader::ChunkCollection& chunkCollection);

            template<class T>
            void ViewAs(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness);
        };
	}
}