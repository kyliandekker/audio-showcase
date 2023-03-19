#include "audio/storage/Sound.h"
#include <uaudio_wave_reader/ChunkCollection.h>

namespace uaudio
{
	namespace storage
	{
		Sound::~Sound()
		{
			free(m_ChunkCollection->data());
			delete m_ChunkCollection;
			free(m_Samples);
		}
	}
}