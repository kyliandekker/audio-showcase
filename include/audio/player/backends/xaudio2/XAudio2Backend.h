#pragma once

#include <vector>

#include "audio/player/backends/AudioBackend.h"
#include "audio/player/backends/xaudio2/XAudio2Channel.h"

struct IXAudio2;
struct IXAudio2MasteringVoice;

namespace uaudio
{
	namespace player
	{
		namespace xaudio2
		{
			class XAudio2Backend : public AudioBackend
			{
			public:
				~XAudio2Backend();
				XAudio2Backend();
				virtual void Update() override;
				UAUDIO_PLAYER_RESULT Play(storage::Sound& a_WaveFile, ChannelHandle& a_Handle) override;

				IXAudio2& GetEngine() const;
				IXAudio2MasteringVoice& GetMasteringVoice() const;

				size_t NumChannels() const override;
				AudioChannel* GetChannel(ChannelHandle& a_Handle) override;
				void RemoveSound(storage::Sound& a_Sound) override;
			private:
				std::vector<XAudio2Channel> m_Channels;

				IXAudio2* m_Engine = nullptr;
				IXAudio2MasteringVoice* m_MasteringVoice = nullptr;
			};
		}
	}
}