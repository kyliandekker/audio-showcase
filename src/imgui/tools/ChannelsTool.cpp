#include "imgui/tools/ChannelsTool.h"

#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <imgui/imgui_helpers.h>
#include <imgui/ImguiDefines.h>
#include <imgui/implot.h>
#include <imgui/imgui_internal.h>
#include <fftw/fftw3.h>
#define M_PI 3.14159265358979323846

#include "audio/player/AudioSystem.h"
#include "audio/player/ChannelHandle.h"
#include "audio/player/AudioChannel.h"
#include "audio/storage/Sound.h"
#include "audio/player/utils.h"
#include "utils/Utils.h"
#include "utils/Logger.h"

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

			ImPlotStyle& pStyle = ImPlot::GetStyle();

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
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve sound from channel %i.", a_Index);
				return;
			}

			if (sound == nullptr)
				return;

			sound->m_Mutex.lock();
			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = sound->m_ChunkCollection->GetChunkFromData(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
			if (UAUDIOWAVEREADERFAILED(result))
			{
				LOGF(logger::LOGSEVERITY_WARNING, "Sound %s has no fmt chunk.", sound->m_Name.c_str());
				sound->m_Mutex.unlock();
				return;
			}

			uaudio::wave_reader::DATA_Chunk data_chunk;
			result = sound->m_ChunkCollection->GetChunkFromData(data_chunk, uaudio::wave_reader::DATA_CHUNK_ID);
			if (UAUDIOWAVEREADERFAILED(result))
			{
				LOGF(logger::LOGSEVERITY_WARNING, "Sound %s has no data chunk.", sound->m_Name.c_str());
				sound->m_Mutex.unlock();
				return;
			}
			std::string sound_hash_id = "##Player_sound_" + std::to_string(sound->m_Hash) + "_";
			std::string sound_name = sound->m_Name;
			double* left_samples = sound->m_LeftSamples;
			double* right_samples = sound->m_RightSamples;
			size_t numSamples = sound->m_NumSamples;
			sound->m_Mutex.unlock();

			float fPos = 0;
			presult = channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_POS, fPos);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve playback position from channel %i.", a_Index);
				return;
			}
			int pos = static_cast<uint32_t>(fPos);
			float final_pos = uaudio::player::utils::PosToSeconds(data_chunk.chunkSize, fmt_chunk.byteRate);
			float seconds = 0;
			presult = channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_S, seconds);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve playback position from channel %i.", a_Index);
				return;
			}

			bool isPlaying = false;
			presult = channel->IsPlaying(isPlaying);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve whether channel %i is currently playing.", a_Index);
				return;
			}

			uint32_t final_pos_slider = isInUse ? data_chunk.chunkSize : 5000;

			if (isInUse)
			{
				std::string channel_name_text = "Channel " + std::to_string(a_Index) + " (" + sound_name + ")" + "##Channel_" + std::to_string(a_Index);
				if (ImGui::CollapsingHeader(channel_name_text.c_str()))
				{
					ImGui::Indent(IMGUI_INDENT);
					ShowValue("Currently playing: ", sound_name.c_str());
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

					std::string eject_button_text = std::string(EJECT) + " Eject Channel##Eject_Channel" + std::to_string(a_Index);
					if (ImGui::Button(eject_button_text.c_str(), ImVec2(120, 25)))
					{
						uaudio::player::audioSystem.m_Update.lock();
						channel->Stop();
						channel->RemoveSound();
						uaudio::player::audioSystem.m_Update.unlock();
						return;
					}

					ImGui::Unindent(IMGUI_INDENT);
				}
			}
			bool active = false;
			presult = channel->IsActive(active);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot check if channel %i is active.", a_Index);
				return;
			}

			if (!active)
				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.00f, 0.66f, 0.18f, 0.05f));








			size_t numSSamples = channel->m_LastDataSize / fmt_chunk.blockAlign;

			if (numSSamples > 0)
			{
				int input_size = numSSamples;
				int output_size = (input_size / 2 + 1);

				double* input_buffer = player::utils::ToSample(channel->m_LastPlayedData, channel->m_LastDataSize, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, numSSamples);

				/*double* hann_buffer = reinterpret_cast<double*>(fftw_malloc(numSSamples * sizeof(double)));
				for (size_t i = 0; i < numSSamples; i++)
				{
					double mult = 0.5f - (0.5f * cos(2 * M_PI * i / (numSSamples - 1)));
					hann_buffer[i] = mult * input_buffer[i];
				}

				fftw_complex* output_buffer = static_cast<fftw_complex*>(fftw_malloc(output_size * sizeof(fftw_complex)));

				int flags = FFTW_ESTIMATE;
				fftw_plan plan = fftw_plan_dft_r2c_1d(input_size,
					hann_buffer,
					output_buffer,
					flags);

				double* freq_s = reinterpret_cast<double*>(malloc(numSSamples / 2 * sizeof(double)));
				double* magn_s = reinterpret_cast<double*>(malloc(numSSamples / 2 * sizeof(double)));

				fftw_execute(plan);

				for (size_t i = 0; i < numSSamples / 2; i++)
				{
					auto re = output_buffer[i][0];
					auto im = output_buffer[i][1];
					auto magn = sqrt(re * re + im * im);
					auto freq = i * (fmt_chunk.sampleRate / (double)(numSSamples / 2));

					freq_s[i] = freq;
					magn_s[i] = magn;
				}

				for (size_t i = 0; i < numSSamples / 2; i++)
				{
					auto scaledMagnitude = magn_s[i] / ((double)numSSamples / 2);
					magn_s[i] = 20 * log10(scaledMagnitude);
				}*/

				std::string graph_eq_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_eq_01";
				if (ImPlot::BeginPlot(graph_eq_name.c_str(), ImVec2(ImGui::GetWindowSize().x - 100, 50), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
				{
					ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
					ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels);
					ImPlot::SetupAxisLimits(ImAxis_Y1, -1.f, 1.f, ImPlotCond_Always);
					ImPlot::SetupAxisLimits(ImAxis_Y1, -1, 1, ImPlotCond_Always);
					ImPlot::PlotLine("", input_buffer, numSSamples);
					ImPlot::EndPlot();
				}

				//free(freq_s);
				//free(magn_s);

				free(input_buffer);
				//fftw_free(hann_buffer);
				//fftw_free(output_buffer);
				//fftw_destroy_plan(plan);
			}

			float height = fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO ? 50 : 100;
			float width = 25;
			size_t new_pos = pos;
			float ex_width = ImGui::GetWindowSize().x - width - 35;
			std::string graph_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_waveform_graph_01";
			std::string graph_name_2 = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_waveform_graph_02";
			ImGui::BeginPlayPlot(new_pos, final_pos_slider, numSamples, left_samples, graph_name.c_str(), ex_width, height, fmt_chunk.blockAlign);

			ImGui::SameLine();

			size_t steps = 10;

			float left_val = player::utils::GetPeak(channel->m_LastPlayedData, channel->m_LastDataSize, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, steps);
			float right_val = player::utils::GetPeak(channel->m_LastPlayedData, channel->m_LastDataSize, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, steps, false);

			left_val = ImLerp(channel->m_LVol, left_val, 0.15f);
			right_val = ImLerp(channel->m_RVol, right_val, 0.15f);

			channel->m_LVol = left_val;
			channel->m_RVol = right_val;

			if (!isPlaying)
			{
				channel->m_LastPlayedData = nullptr;
				channel->m_LastDataSize = 0;
			}

			size_t actual_steps = steps - (10 * 0.15f);

			float meter_width = fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO ? 5.25f : 12.5f;
			std::string meter_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_meter_01";
			std::string meter_name_2 = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_meter_02";
			ImGui::UvMeter(meter_name.c_str(), ImVec2(meter_width, 90), &left_val, 0, actual_steps, actual_steps);
			if (fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO)
			{
				ImGui::SameLine();
				ImGui::UvMeter(meter_name_2.c_str(), ImVec2(meter_width, 90), &right_val, 0, actual_steps, actual_steps);
			}

			if (fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO)
				ImGui::BeginPlayPlot(new_pos, final_pos_slider, numSamples, right_samples, graph_name_2.c_str(), ex_width, height, fmt_chunk.blockAlign);
			if (!active)
				ImPlot::PopStyleColor();

			if (new_pos != pos)
			{
				channel->SetPos(new_pos);

				if (!isPlaying)
					channel->PlayRanged(new_pos, static_cast<uint32_t>(9600));
			}

			std::string on_off_button_text = "##OnOff_Channel_" + std::to_string(a_Index);
			if (ImGui::OnOffButton(on_off_button_text.c_str(), &active, ImVec2(15, 15)))
				channel->SetActive(active);

			ImGui::SameLine();
			float panning = 0.0f;
			presult = channel->GetPanning(panning);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume from panning %i.", a_Index);
				return;
			}
			std::string panning_tooltip_text = std::string(PANNING) + " Panning (affects channel " + std::to_string(a_Index) + ")";
			std::string panning_text = "##Panning_Channel_" + std::to_string(a_Index);
			if (ImGui::Knob(panning_text.c_str(), &panning, -1, 1, ImVec2(15, 15), panning_tooltip_text.c_str(), 0.0f))
				channel->SetPanning(panning);

			ImGui::SameLine();
			float volume = 1.0f;
			presult = channel->GetVolume(volume);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume from channel %i.", a_Index);
				return;
			}
			std::string volume_tooltip_text = std::string(VOLUME_UP) + " Volume (affects channel " + std::to_string(a_Index) + ")";
			std::string volume_text = "##Volume_Channel_" + std::to_string(a_Index);
			if (ImGui::Knob(volume_text.c_str(), &volume, 0, 1, ImVec2(15, 15), volume_tooltip_text.c_str(), 1.0f))
				channel->SetVolume(volume);

			std::string player_text = std::string("###Player_" + std::to_string(a_Index));

			ImVec2 temp = ImVec2(ImGui::GetWindowSize().x - 45, 100);
			ImGui::InvisibleButton("", ImVec2((temp.x / 2) - pStyle.PlotPadding.x - style.ItemInnerSpacing.x - style.ItemInnerSpacing.x - style.ItemInnerSpacing.x - 25 - 25 - 35, 35 + style.ItemInnerSpacing.y));
			ImGui::SameLine();

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
				int32_t prev_pos = pos - static_cast<int>(9600);
				prev_pos = clamp<int32_t>(prev_pos, 0, data_chunk.chunkSize);
				channel->SetPos(prev_pos);
				channel->Pause();
				channel->PlayRanged(prev_pos, static_cast<int>(9600));
			}

			ImGui::SameLine();

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y - 5));

			ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 25.0f);
			ImVec4 color = ImGui::GetStyleColorVec4(ImGuiCol_Slider);
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			color = ImVec4(color.x - 0.05f, color.y - 0.05f, color.z, color.w);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
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
			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			ImGui::SameLine();

			ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x, ImGui::GetCursorPos().y + 5));

			std::string right_button_text = std::string(FORWARD) + "##Right_Sound_" + std::to_string(a_Index);
			if (ImGui::InvisButton(right_button_text.c_str(), ImVec2(25, 25)))
			{
				int32_t next_pos = pos + static_cast<int>(9600);
				next_pos = clamp<int32_t>(next_pos, 0, data_chunk.chunkSize);
				channel->SetPos(next_pos);
				channel->Pause();
				channel->PlayRanged(next_pos, static_cast<int>(9600));
			}

			ImGui::SameLine();

			bool isLooping = false;
			presult = channel->IsLooping(isLooping);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve whether channel %i has looping turned on.", a_Index);
				return;
			}
			std::string loop_button_text = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
			if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
				channel->SetLooping(isLooping);

			ImGui::Separator();
		}
	}
}