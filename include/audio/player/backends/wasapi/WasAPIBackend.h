#pragma once

#include <vector>

#include "audio/player/backends/AudioBackend.h"

class IMMDevice;
class IAudioClient;
class IAudioRenderClient;

namespace uaudio
{
	namespace player
	{
		namespace wasapi
		{
			class WasAPIChannel;

			class WasAPIBackend : public AudioBackend
			{
			public:
				WasAPIBackend();
				~WasAPIBackend();

				virtual void Update() override;
				UAUDIO_PLAYER_RESULT Play(storage::Sound& a_WaveFile, ChannelHandle& a_Handle) override;

				size_t NumChannels() const override;
				AudioChannel* GetChannel(ChannelHandle& a_Handle) override;
				void RemoveSound(storage::Sound& a_Sound) override;
			private:
				IMMDevice* m_Device = nullptr;
				IAudioClient* m_AudioClient = nullptr;
				IAudioRenderClient* m_AudioRenderClient = nullptr;
			};
		}
	}
}