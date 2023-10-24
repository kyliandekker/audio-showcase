#pragma once

#include <vector>

#include "audio/player/backends/AudioBackend.h"
#include "audio/player/backends/wasapi/WasAPIChannel.h"

struct IMMDevice;

namespace uaudio
{
	namespace storage
	{
		class Sound;
	}
	namespace player
	{
		namespace wasapi
		{
			class WasAPIBackend : public AudioBackend
			{
			public:
				WasAPIBackend();
				~WasAPIBackend();

				virtual void Update() override;
				UAUDIO_PLAYER_RESULT Play(storage::Sound& a_Sound, ChannelHandle& a_Handle) override;

				size_t NumChannels() const override;
				AudioChannel* GetChannel(ChannelHandle& a_Handle) override;
				void RemoveSound(storage::Sound& a_Sound) override;

				IMMDevice& GetDevice() const;
			private:
				std::vector<WasAPIChannel> m_Channels;

				IMMDevice* m_Device = nullptr;
			};
		}
	}
}