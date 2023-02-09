#include "audio/player/Sound.h"

namespace uaudio
{
	namespace player
	{
		Sound::~Sound()
		{
			delete m_ChunkCollection;
			free(m_Samples);
		}
	}
}