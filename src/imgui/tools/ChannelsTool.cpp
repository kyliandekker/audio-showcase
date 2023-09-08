#include "imgui/tools/ChannelsTool.h"

#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <imgui/imgui_helpers.h>
#include <imgui/ImguiDefines.h>

#include "audio/player/AudioSystem.h"
#include "audio/player/ChannelHandle.h"
#include "audio/player/AudioChannel.h"
#include "audio/storage/Sound.h"
#include "audio/player/utils.h"
#include "utils/Utils.h"

namespace uaudio
{
	namespace imgui
	{
		ChannelsTool::ChannelsTool() : BaseTool(0, "Channels", "Channels")
		{ }

		void ChannelsTool::Render()
		{
			size_t size = 0;
			if (uaudio::player::audioSystem.NumChannels(size) != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK)
				return;

			for (uint32_t i = 0; i < size; i++)
				RenderChannel(i);
		}

		void ChannelsTool::RenderChannel(player::ChannelHandle a_Index)
		{
			uaudio::player::AudioChannel* channel = nullptr;
			uaudio::player::audioSystem.GetChannel(a_Index, channel);

			bool isInUse = false;
			channel->IsInUse(isInUse);
			if (!isInUse)
				return;

			uaudio::storage::Sound* sound;
			channel->GetSound(sound);
			sound->m_Mutex.lock();

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);

			bool active = false;
			channel->IsActive(active);
			std::string on_off_button_text = "##OnOff_Channel_" + std::to_string(a_Index);
			if (ImGui::OnOffButton(on_off_button_text.c_str(), &active, ImVec2(25, 25)))
				channel->SetActive(active);

			ImGui::SameLine();
			float panning = 0.0f;
			channel->GetPanning(panning);
			std::string panning_tooltip_text = std::string(PANNING) + " Panning (affects channel " + std::to_string(a_Index) + ")";
			std::string panning_text = "##Panning_Channel_" + std::to_string(a_Index);
			if (ImGui::Knob(panning_text.c_str(), &panning, -1, 1, ImVec2(25, 25), panning_tooltip_text.c_str(), 0.0f))
				channel->SetPanning(panning);

			ImGui::SameLine();
			float volume = 1.0f;
			channel->GetVolume(volume);
			std::string volume_tooltip_text = std::string(VOLUME_UP) + " Volume (affects channel " + std::to_string(a_Index) + ")";
			std::string volume_text = "##Volume_Channel_" + std::to_string(a_Index);
			if (ImGui::Knob(volume_text.c_str(), &volume, 0, 1, ImVec2(25, 25), volume_tooltip_text.c_str(), 1.0f))
				channel->SetVolume(volume);

			uaudio::wave_reader::DATA_Chunk data_chunk;
			sound->m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);

			float fPos = 0;
			channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_POS, fPos);
			int pos = static_cast<uint32_t>(fPos);
			float final_pos = uaudio::player::utils::PosToSeconds(data_chunk.chunkSize, fmt_chunk.byteRate);
			float seconds = 0;
			channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_S, seconds);
			ImGui::Text("%s", std::string(
				uaudio::player::utils::FormatDuration(seconds, false) +
				"/" +
				uaudio::player::utils::FormatDuration(final_pos, false))
				.c_str());
			uint32_t final_pos_slider = isInUse ? data_chunk.chunkSize : 5000;
			std::string player_text = std::string("###Player_" + std::to_string(a_Index));

			bool isPlaying = false;
			channel->IsPlaying(isPlaying);

			uint32_t buffersize = 0;
			uaudio::player::audioSystem.GetBufferSize(buffersize);

			if (ImGui::SliderInt(player_text.c_str(), &pos, 0, static_cast<int>(final_pos_slider), ""))
			{
				uint32_t new_pos = pos % static_cast<int>(buffersize);
				uint32_t final_new_pos = pos - new_pos;
				final_new_pos = clamp<uint32_t>(final_new_pos, 0, data_chunk.chunkSize);
				channel->SetPos(final_new_pos);
				
				if (!isPlaying)
					channel->PlayRanged(final_new_pos, static_cast<int>(buffersize));
			}

			if (isPlaying)
			{
				std::string pause_button_text = std::string(PAUSE) + "##Pause_Sound_" + std::to_string(a_Index);
				if (ImGui::Button(pause_button_text.c_str(), ImVec2(25, 25)))
					channel->Pause();
			}
			else
			{
				std::string play_button_text = std::string(PLAY) + "##Play_Sound_" + std::to_string(a_Index);
				if (ImGui::Button(play_button_text.c_str(), ImVec2(25, 25)))
					channel->Play();
			}

			ImGui::SameLine();
			std::string left_button_text = std::string(LEFT) + "##Left_Sound_" + std::to_string(a_Index);
			if (ImGui::Button(left_button_text.c_str(), ImVec2(25, 25)))
			{
				int32_t prev_pos = pos - static_cast<int>(buffersize);
				prev_pos = clamp<int32_t>(prev_pos, 0, data_chunk.chunkSize);
				channel->SetPos(prev_pos);
				channel->Pause();
				channel->PlayRanged(prev_pos, static_cast<int>(buffersize));
			}

			ImGui::SameLine();
			std::string stop_button_text = std::string(STOP) + "##Stop_Sound_" + std::to_string(a_Index);
			if (ImGui::Button(stop_button_text.c_str(), ImVec2(25, 25)))
			{
				channel->SetPos(0);
				channel->Pause();
			}

			ImGui::SameLine();
			std::string right_button_text = std::string(RIGHT) + "##Right_Sound_" + std::to_string(a_Index);
			if (ImGui::Button(right_button_text.c_str(), ImVec2(25, 25)))
			{
				int32_t next_pos = pos + static_cast<int>(buffersize);
				next_pos = clamp<int32_t>(next_pos, 0, data_chunk.chunkSize);
				channel->SetPos(next_pos);
				channel->Pause();
				channel->PlayRanged(next_pos, static_cast<int>(buffersize));
			}

			ImGui::SameLine();
			bool isLooping = false;
			channel->IsLooping(isLooping);
			std::string loop_button_text = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
			if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
				channel->SetLooping(isLooping);

			if (isInUse)
			{
				std::string channel_name_text = "Channel " + std::to_string(a_Index) + " (" + sound->m_Name + ")" + "##Channel_" + std::to_string(a_Index);
				if (ImGui::CollapsingHeader(channel_name_text.c_str()))
				{
					ImGui::Indent(IMGUI_INDENT);
					ShowValue("Currently playing: ", sound->m_Name.c_str());
					ShowValue("Progress: ", std::string(
						uaudio::player::utils::FormatDuration(fPos / static_cast<float>(fmt_chunk.byteRate), false) +
						"/" +
						uaudio::player::utils::FormatDuration(uaudio::player::utils::GetDuration(data_chunk.chunkSize, fmt_chunk.byteRate), false))
						.c_str());

					ShowValue("Time Left: ", std::string(uaudio::player::utils::FormatDuration(
						uaudio::player::utils::GetDuration(data_chunk.chunkSize, fmt_chunk.byteRate) - (static_cast<float>(fPos) / static_cast<float>(fmt_chunk.byteRate)), false)).c_str());

					ShowValue("Progress (position): ", std::string(std::to_string(static_cast<int>(fPos)) +
						"/" +
						std::to_string(data_chunk.chunkSize)
						).c_str());

					ImGui::Unindent(IMGUI_INDENT);
				}
			}
			ImGui::Separator();
			sound->m_Mutex.unlock();
		}
	}
}