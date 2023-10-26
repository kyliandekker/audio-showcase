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
			struct buffer
			{
				unsigned char* data = nullptr;
				size_t size = 0;

				buffer(unsigned char* data, size_t size)
				{
					this->data = data;
					this->size = size;
				}
			};

			class XAudio2Backend;

			class XAudio2Channel : public AudioChannel
			{
			private:
				std::queue<buffer> m_DataBuffers;
				size_t total_buffer_size = 0;

				IXAudio2SourceVoice* m_SourceVoice = nullptr;
				XAudio2Backend* m_Backend = nullptr;

				friend XAudio2Backend;

				UAUDIO_PLAYER_RESULT PlayRanged(uint32_t a_StartPos, uint32_t a_Size) override;
				UAUDIO_PLAYER_RESULT PlayBuffer(const unsigned char* a_DataBuffer, uint32_t a_Size) const;
				uint32_t m_LeftOver = 0;
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