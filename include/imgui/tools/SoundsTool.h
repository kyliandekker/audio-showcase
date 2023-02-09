#pragma once

#include "imgui/tools/BaseTool.h"

namespace uaudio
{
	namespace player
	{
		class Sound;
	}

	namespace imgui
	{
		class SoundsTool : public BaseTool
		{
		public:
			SoundsTool();
			void Render() override;
			void RenderSound(uaudio::player::Sound& a_Sound);
		};
	}
}