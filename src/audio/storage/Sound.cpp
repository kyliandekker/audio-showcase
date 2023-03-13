#include "audio/storage/Sound.h"

namespace uaudio
{
	namespace storage
	{
		Sound::~Sound()
		{
			delete m_ChunkCollection;
			free(m_Samples);
		}
	}
}