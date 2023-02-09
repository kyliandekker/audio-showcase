#pragma once

#include "imgui/tools/BaseTool.h"

namespace uaudio
{
	namespace player
	{
		class Sound;
	}

	namespace wave_reader
	{
		class ChunkCollection;
	}

	namespace imgui
	{
		class SoundsTool : public BaseTool
		{
		public:
			SoundsTool();
			void Render() override;
            void ShowBaseChunk(char* a_ChunkId, uaudio::wave_reader::ChunkCollection& chunkCollection);
			void RenderSound(uaudio::player::Sound& a_Sound);
		};
	}
}