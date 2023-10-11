#include "imgui/tools/ChannelsTool.h"

#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <imgui/imgui_helpers.h>
#include <imgui/ImguiDefines.h>
#include <imgui/implot.h>

#include "audio/player/AudioSystem.h"
#include "audio/player/ChannelHandle.h"
#include "audio/player/AudioChannel.h"
#include "audio/storage/Sound.h"
#include "audio/player/utils.h"
#include "utils/Utils.h"
#include "utils/Logger.h"
#include <imgui/imgui_internal.h>

namespace uaudio
{
	namespace imgui
	{
		ChannelsTool::ChannelsTool() : BaseTool(0, "Channels", "Channels")
		{ }

		void ChannelsTool::Render()
		{
			size_t size = 0;
			uaudio::player::UAUDIO_PLAYER_RESULT result = uaudio::player::audioSystem.NumChannels(size);
			if (UAUDIOPLAYERFAILED(result))
				return;

			for (uint32_t i = 0; i < size; i++)
				RenderChannel(i);
		}

		void ChannelsTool::RenderChannel(player::ChannelHandle a_Index)
		{
			ImGuiStyle& style = ImGui::GetStyle();

			ImVec2 CursorPos = ImGui::GetCursorPos();
			ImVec4 c = style.Colors[ImGuiCol_Header];
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			draw_list->AddRectFilled(CursorPos, ImVec2(CursorPos.x + CursorPos.x, CursorPos.y + 50), ImGui::GetColorU32(c));

			uaudio::player::AudioChannel* channel = nullptr;
			uaudio::player::UAUDIO_PLAYER_RESULT presult = uaudio::player::audioSystem.GetChannel(a_Index, channel);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot get channel %i.", a_Index);
				return;
			}

			bool isInUse = false;
			presult = channel->IsInUse(isInUse);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot check if channel %i is in use.", a_Index);
				return;
			}
			if (!isInUse)
				return;

			uaudio::storage::Sound* sound;
			presult = channel->GetSound(sound);

			if (sound == nullptr)
				return;

			if (sound)
				sound->m_Mutex.lock();
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve sound from channel %i.", a_Index);
				return;
			}

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
			if (UAUDIOWAVEREADERFAILED(result))
			{
				LOGF(logger::LOGSEVERITY_WARNING, "Sound %s has no fmt chunk.", sound->m_Name.c_str());
				sound->m_Mutex.unlock();
				return;
			}
			
			bool active = false;
			presult = channel->IsActive(active);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot check if channel %i is active.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}
			std::string on_off_button_text = "##OnOff_Channel_" + std::to_string(a_Index);
			if (ImGui::OnOffButton(on_off_button_text.c_str(), &active, ImVec2(25, 25)))
				channel->SetActive(active);

			ImGui::SameLine();
			float panning = 0.0f;
			presult = channel->GetPanning(panning);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume from panning %i.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}
			std::string panning_tooltip_text = std::string(PANNING) + " Panning (affects channel " + std::to_string(a_Index) + ")";
			std::string panning_text = "##Panning_Channel_" + std::to_string(a_Index);
			if (ImGui::Knob(panning_text.c_str(), &panning, -1, 1, ImVec2(25, 25), panning_tooltip_text.c_str(), 0.0f))
				channel->SetPanning(panning);

			ImGui::SameLine();
			float volume = 1.0f;
			presult = channel->GetVolume(volume);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume from channel %i.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}
			std::string volume_tooltip_text = std::string(VOLUME_UP) + " Volume (affects channel " + std::to_string(a_Index) + ")";
			std::string volume_text = "##Volume_Channel_" + std::to_string(a_Index);
			if (ImGui::Knob(volume_text.c_str(), &volume, 0, 1, ImVec2(25, 25), volume_tooltip_text.c_str(), 1.0f))
				channel->SetVolume(volume);

			uaudio::wave_reader::DATA_Chunk data_chunk;
			result = sound->m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
			if (UAUDIOWAVEREADERFAILED(result))
			{
				LOGF(logger::LOGSEVERITY_WARNING, "Sound %s has no data chunk.", sound->m_Name.c_str());
				sound->m_Mutex.unlock();
				return;
			}

			float fPos = 0;
			presult = channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_POS, fPos);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve playback position from channel %i.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}
			int pos = static_cast<uint32_t>(fPos);
			float final_pos = uaudio::player::utils::PosToSeconds(data_chunk.chunkSize, fmt_chunk.byteRate);
			float seconds = 0;
			presult = channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_S, seconds);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve playback position from channel %i.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}

			uint32_t final_pos_slider = isInUse ? data_chunk.chunkSize : 5000;
			std::string player_text = std::string("###Player_" + std::to_string(a_Index));

			bool isPlaying = false;
			presult = channel->IsPlaying(isPlaying);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve whether channel %i is currently playing.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}

			uint32_t buffersize = 0;
			presult = uaudio::player::audioSystem.GetBufferSize(buffersize);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve audio system buffer size.");
				sound->m_Mutex.unlock();
				return;
			}

			std::string sound_hash_id = "##Player_sound_" + std::to_string(sound->m_Hash) + "_";
			std::string graph_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_waveform_graph";

			sound->m_Mutex.unlock();

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
			ImGui::Unindent(IMGUI_INDENT);

			ImVec2 plotSize;

			size_t new_pos = ImGui::BeginPlayPlot(pos, final_pos_slider, sound->m_NumSamples, sound->m_Samples, graph_name.c_str(), plotSize, std::string(
				uaudio::player::utils::FormatDuration(seconds, false) +
				"/" +
				uaudio::player::utils::FormatDuration(final_pos, false))
				.c_str());
			ImGui::Indent(IMGUI_INDENT);
			if (new_pos != pos)
			{
				uint32_t left_over = new_pos % static_cast<int>(buffersize);
				uint32_t final_new_pos = new_pos - left_over;
				channel->SetPos(final_new_pos);

				if (!isPlaying)
					channel->PlayRanged(final_new_pos, static_cast<int>(buffersize));
			}
			sound->m_Mutex.lock();

			std::string stop_button_text = std::string(STOP) + "##Stop_Sound_" + std::to_string(a_Index);
			if (ImGui::InvisButton(stop_button_text.c_str(), ImVec2(25, 25)))
			{
				channel->SetPos(0);
				channel->Pause();
			}

			ImGui::SameLine();

			std::string left_button_text = std::string(BACKWARD) + "##Left_Sound_" + std::to_string(a_Index);
			if (ImGui::InvisButton(left_button_text.c_str(), ImVec2(25, 25)))
			{
				int32_t prev_pos = pos - static_cast<int>(buffersize);
				prev_pos = clamp<int32_t>(prev_pos, 0, data_chunk.chunkSize);
				channel->SetPos(prev_pos);
				channel->Pause();
				channel->PlayRanged(prev_pos, static_cast<int>(buffersize));
			}

			ImGui::SameLine();

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.0f);
			if (isPlaying)
			{
				std::string pause_button_text = std::string(PAUSE) + "##Pause_Sound_" + std::to_string(a_Index);
				if (ImGui::Button(pause_button_text.c_str(), ImVec2(35, 35)))
					channel->Pause();
			}
			else
			{
				std::string play_button_text = std::string(PLAY) + "##Play_Sound_" + std::to_string(a_Index);
				if (ImGui::Button(play_button_text.c_str(), ImVec2(35, 35)))
					channel->Play();
			}
			ImGui::PopStyleVar();

			ImGui::SameLine();

			std::string right_button_text = std::string(FORWARD) + "##Right_Sound_" + std::to_string(a_Index);
			if (ImGui::InvisButton(right_button_text.c_str(), ImVec2(25, 25)))
			{
				int32_t next_pos = pos + static_cast<int>(buffersize);
				next_pos = clamp<int32_t>(next_pos, 0, data_chunk.chunkSize);
				channel->SetPos(next_pos);
				channel->Pause();
				channel->PlayRanged(next_pos, static_cast<int>(buffersize));
			}

			ImGui::SameLine();

			bool isLooping = false;
			presult = channel->IsLooping(isLooping);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve whether channel %i has looping turned on.", a_Index);
				sound->m_Mutex.unlock();
				return;
			}
			std::string loop_button_text = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
			if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
				channel->SetLooping(isLooping);

			ImGui::Separator();
			sound->m_Mutex.unlock();
		}
	}
}