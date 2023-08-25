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
			class XAudio2Channel;

			class XAudio2Backend : public AudioBackend
			{
				virtual void Update() override;
			private:
				IXAudio2* m_Engine = nullptr;
				IXAudio2MasteringVoice* m_MasterVoice = nullptr;
			};
		}
	}
}