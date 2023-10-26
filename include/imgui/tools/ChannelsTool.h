#pragma once

#include "imgui/tools/BaseTool.h"

namespace uaudio
{
	namespace player
	{
		struct ChannelHandle;
	}

	struct fft_analysis_settings
	{
		float graph_height = 100;
		std::string option_name = "";
		uint32_t numSamples = 0;
		float min_y, max_y;
	};

	enum fft_option
	{
		Sample,
		Played_Sample,
		Scrolling_Sample,
		Led_Bars,
		EQ,
	};

	namespace imgui
	{
		class ChannelsTool : public BaseTool
		{
		public:
			ChannelsTool();
			void Render() override;
		private:
			void RenderChannel(player::ChannelHandle a_Index);

			fft_option m_SelectedSP = Sample;
        };
	}
}