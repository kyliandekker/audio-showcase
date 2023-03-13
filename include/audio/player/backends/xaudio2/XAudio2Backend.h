#pragma once

#include <xaudio2.h>
#include <vector>

#include "audio/player/backends/AudioBackend.h"

namespace uaudio
{
	namespace player
	{
		namespace xaudio2
		{
			class XAudio2Backend : public AudioBackend
			{
			private:
				IXAudio2* m_Engine = nullptr;
				IXAudio2MasteringVoice* m_MasterVoice = nullptr;

				std::vector<XAudio2Channel> m_Channels;
			};
		}
	}
}