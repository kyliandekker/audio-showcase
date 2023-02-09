#include "imgui/tools/SoundsTool.h"

#include <vector>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <imgui/imgui_helpers.h>

#include "audio/player/AudioSystem.h"
#include "audio/player/Sound.h"
#include "imgui/ImguiDefines.h"

namespace uaudio
{
	namespace imgui
	{
		SoundsTool::SoundsTool() : BaseTool(0, "Sounds", "Sounds")
		{
		}

		void SoundsTool::Render()
		{
			std::vector<uaudio::player::Sound*> sounds = uaudio::player::audioSystem.GetSounds();
			for (const auto sound : sounds)
				RenderSound(*sound);
		}

		void SoundsTool::RenderSound(uaudio::player::Sound& a_Sound)
		{
			//uaudio::WaveFormat* a_WaveFile = nullptr;
			//uaudio::SoundSys.FindSound(a_WaveFile, a_SoundHash);

			std::string sound_hash_id = "##sound_" + std::to_string(a_Sound.m_Hash) + "_";

			std::string sound_header = a_Sound.m_Name + sound_hash_id + "sound_collapse";
			if (!ImGui::CollapsingHeader(sound_header.c_str()))
				return;

			uaudio::wave_reader::ChunkCollection& chunkCollection = *a_Sound.m_ChunkCollection;
			bool hasFmtChunk = false, hasDataChunk = false;
			chunkCollection.HasChunk(hasFmtChunk, uaudio::wave_reader::FMT_CHUNK_ID);
			chunkCollection.HasChunk(hasDataChunk, uaudio::wave_reader::DATA_CHUNK_ID);

			if (hasFmtChunk && hasDataChunk)
			{
				std::string play_button_text = std::string(PLAY) + " Play" + sound_hash_id + "play_button";
				if (ImGui::Button(play_button_text.c_str()))
				{
					int32_t channel_index = 0;
					//audios.CreateSoundChannel(channel_index, a_SoundHash);
					//uaudio::Channel* channel = nullptr;
					//m_AudioSystem.GetSoundChannel(channel, channel_index);
					//channel->Play();
				}
			}

			ImGui::SameLine();
			std::string remove_sound_text = std::string(MINUS) + " Unload" + sound_hash_id + "unload_sound_button";
			if (ImGui::Button(remove_sound_text.c_str()))
			{
				uaudio::player::audioSystem.UnloadSound(a_Sound.m_Hash);
				return;
			}

			ImGui::SameLine();
			std::string save_sound_text = std::string(SAVE) + " Save" + sound_hash_id + "save_sound_button";
			if (ImGui::Button(save_sound_text.c_str()))
			{
				// SaveFile(a_WaveFile);
			}

			ImGui::SameLine();
			bool isLooping = false;
			std::string loop_button_text = std::string(RETRY) + sound_hash_id + "loop_sound";
			if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
			{

			}
		}
	}
}