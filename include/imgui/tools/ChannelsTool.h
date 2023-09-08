#pragma once

#include "imgui/tools/BaseTool.h"

namespace uaudio
{
	namespace player
	{
		struct ChannelHandle;
	}
	namespace imgui
	{
		class ChannelsTool : public BaseTool
		{
		public:
			ChannelsTool();
			void Render() override;
		private:
			void RenderChannel(player::ChannelHandle a_Index);
        };
	}
}