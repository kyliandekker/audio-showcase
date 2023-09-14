#pragma once

#include "audio/player/AudioChannel.h"
#include <queue>

struct IXAudio2SourceVoice;

namespace uaudio
{
	namespace player
	{
		namespace xaudio2
		{
			class XAudio2Backend;

			class XAudio2Channel : public AudioChannel
			{
			private:
				std::queue<unsigned char*> m_DataBuffers;

				IXAudio2SourceVoice* m_SourceVoice = nullptr;
				XAudio2Backend* m_Backend = nullptr;

				UAUDIO_PLAYER_RESULT PlayRanged(uint32_t a_StartPos, uint32_t a_Size) override;
				UAUDIO_PLAYER_RESULT PlayBuffer(const unsigned char* a_DataBuffer, uint32_t a_Size) const;
			public:
				XAudio2Channel();
				XAudio2Channel(XAudio2Backend& a_Backend);
				~XAudio2Channel();

				UAUDIO_PLAYER_RESULT SetSound(storage::Sound& a_Sound) override;
				UAUDIO_PLAYER_RESULT Stop() override;
				UAUDIO_PLAYER_RESULT Update() override;
			};
		}
	}
}