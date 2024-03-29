#pragma once

#include "audio/player/AudioChannel.h"
#include <queue>

struct IAudioRenderClient;
struct IAudioClient;

namespace uaudio
{
	namespace player
	{
		namespace wasapi
		{
			class WasAPIBackend;

			class WasAPIChannel : public AudioChannel
			{
			private:
				IAudioClient* m_AudioClient = nullptr;
				IAudioRenderClient* m_RenderClient = nullptr;

				WasAPIBackend* m_Backend = nullptr;

				UAUDIO_PLAYER_RESULT PlayRanged(uint32_t a_StartPos, uint32_t a_Size) override;
				UAUDIO_PLAYER_RESULT PlayBuffer(uint32_t a_Size) const;

				friend WasAPIBackend;
			public:
				WasAPIChannel();
				WasAPIChannel(WasAPIBackend& a_Backend);
				~WasAPIChannel();

				UAUDIO_PLAYER_RESULT SetSound(storage::Sound& a_Sound) override;
				UAUDIO_PLAYER_RESULT Stop() override;
				UAUDIO_PLAYER_RESULT Update() override;
			};
		}
	}
}