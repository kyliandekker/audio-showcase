#pragma once

#include <cstdint>

#include "Defines.h"
#include "UAUDIO_PLAYER_RESULT.h"

namespace uaudio
{
	namespace storage
	{
		class Sound;
	}
	namespace player
	{
		class AudioChannel
		{
		public:
			UAUDIO_PLAYER_RESULT SetVolume(float a_Volume);
			UAUDIO_PLAYER_RESULT GetVolume(float& a_Volume) const;

			UAUDIO_PLAYER_RESULT SetPanning(float a_Panning);
			UAUDIO_PLAYER_RESULT GetPanning(float& a_Panning) const;

			UAUDIO_PLAYER_RESULT IsPlaying(bool& a_IsPlaying) const;
			UAUDIO_PLAYER_RESULT SetActive(bool a_Active);
			UAUDIO_PLAYER_RESULT IsActive(bool& a_Active) const;

			UAUDIO_PLAYER_RESULT IsInUse(bool& a_IsInUse) const;
			virtual UAUDIO_PLAYER_RESULT SetSound(storage::Sound& a_Sound);
			UAUDIO_PLAYER_RESULT GetSound(storage::Sound*& a_Sound);
			UAUDIO_PLAYER_RESULT RemoveSound();

			virtual UAUDIO_PLAYER_RESULT PlayRanged(uint32_t a_StartPos, uint32_t a_Size) = 0;

			UAUDIO_PLAYER_RESULT SetLooping(bool a_Looping);
			UAUDIO_PLAYER_RESULT IsLooping(bool& a_Looping) const;

			UAUDIO_PLAYER_RESULT Play();
			UAUDIO_PLAYER_RESULT Pause();
			virtual UAUDIO_PLAYER_RESULT Stop();
			virtual UAUDIO_PLAYER_RESULT Update();
			UAUDIO_PLAYER_RESULT SetPos(uint32_t a_Pos);
			UAUDIO_PLAYER_RESULT GetPos(TIMEUNIT a_TimeUnit, float& a_Pos) const;

			UAUDIO_PLAYER_RESULT GetSoundBufferSize(uint32_t& a_BufferSize) const;
		protected:
			storage::Sound* m_Sound = nullptr;

			float m_Volume = 1.0f;
			float m_Panning = 0.0f;

			// Difference between Active and Playing:
			// Playing is whether the channel gets updated and actually moves through the audio data.
			// Active means whether the audio channel is actually active or turned off. It can still be "playing" data.
			bool m_IsPlaying = false, m_Active = true;

			bool m_Looping = false;
			uint32_t m_CurrentPos = 0;
		};
	}
}