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

double highest = 0.0f;
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

		double hanning(int i, int nn)
		{
			return (0.5 * (1.0 - cos(2.0 * M_PI * (double)i / (double)(nn - 1))));
		}

		double hamming(int i, int nn)
		{
			return (0.54 - 0.46 * cos(2.0 * M_PI * (double)i / (double)(nn - 1)));
		}

		double blackman(int i, int nn)
		{
			return (0.42 - 0.5 * cos(2.0 * M_PI * (double)i / (double)(nn - 1))
				+ 0.08 * cos(4.0 * M_PI * (double)i / (double)(nn - 1)));
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
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot get channel %i.", static_cast<int>(a_Index));
				return;
			}

			bool isInUse = false;
			presult = channel->IsInUse(isInUse);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot check if channel %i is in use.", static_cast<int>(a_Index));
				return;
			}
			if (!isInUse)
				return;

			uaudio::storage::Sound* sound;
			presult = channel->GetSound(sound);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve sound from channel %i.", static_cast<int>(a_Index));
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
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve playback position from channel %i.", static_cast<int>(a_Index));
				return;
			}
			uint32_t pos = static_cast<uint32_t>(fPos);
			float seconds = 0;
			presult = channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_S, seconds);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve playback position from channel %i.", static_cast<int>(a_Index));
				return;
			}

			bool isPlaying = false;
			presult = channel->IsPlaying(isPlaying);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve whether channel %i is currently playing.", static_cast<int>(a_Index));
				return;
			}

			uint32_t final_pos_slider = isInUse ? data_chunk.ChunkSize() : 5000;

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
						uaudio::player::utils::FormatDuration(uaudio::player::utils::GetDuration(data_chunk.ChunkSize(), fmt_chunk.byteRate), false))
						.c_str());

					ShowValue("Time Left: ", std::string(uaudio::player::utils::FormatDuration(
						uaudio::player::utils::GetDuration(data_chunk.ChunkSize(), fmt_chunk.byteRate) - (static_cast<float>(fPos) / static_cast<float>(fmt_chunk.byteRate)), false)).c_str());

					ShowValue("Progress (position): ", std::string(std::to_string(static_cast<int>(fPos)) +
						"/" +
						std::to_string(data_chunk.ChunkSize())
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
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot check if channel %i is active.", static_cast<int>(a_Index));
				return;
			}

			if (!active)
				ImPlot::PushStyleColor(ImPlotCol_Line, ImVec4(1.00f, 0.66f, 0.18f, 0.05f));











			std::string sp_name_text = "Signal Processing##SignalProcessing_" + std::to_string(a_Index);
			if (ImGui::CollapsingHeader(sp_name_text.c_str()))
			{
				const int BLOCK_SIZE_OPTIONS = 7;
				size_t block_size_options[BLOCK_SIZE_OPTIONS] =
				{
					32,
					64,
					128,
					256,
					512,
					1024,
					2048,
				};

				std::string view_block_size_options[BLOCK_SIZE_OPTIONS];
				for (size_t i = 0; i < BLOCK_SIZE_OPTIONS; i++)
					view_block_size_options[i] = std::to_string(block_size_options[i]);

				const std::string block_size_text = "Block Size";
				const std::string block_size_text_id = "##BlockSize_" + std::to_string(a_Index) + " (" + sound_name + ")" + "##BlockSize_" + std::to_string(a_Index);
				ImGui::Text("%s", block_size_text.c_str());
				ImGui::SameLine();
				if (ImGui::BeginCombo(block_size_text_id.c_str(), view_block_size_options[m_BlockSize].c_str(), ImGuiComboFlags_PopupAlignLeft))
				{
					for (uint32_t n = 0; n < BLOCK_SIZE_OPTIONS; n++)
					{
						const bool is_selected = n == m_BlockSize;
						if (ImGui::Selectable(view_block_size_options[n].c_str(), is_selected))
							m_BlockSize = static_cast<fft_option>(n);
					}
					ImGui::EndCombo();
				}

				const int OPTIONS_AMNT = 6;
				size_t amntSamples = fmt_chunk.blockAlign * block_size_options[m_BlockSize];

				dsp_analysis_settings dsp_options[OPTIONS_AMNT] = {
					{ 100, "Sample", amntSamples, -1, 1, amntSamples },
					{ 100, "Played Sample", (channel->m_LastDataSize * fmt_chunk.blockAlign), -1, 1, amntSamples },
					{ 100, "Scrolling sample", fmt_chunk.sampleRate, -2, 2, amntSamples },
					{ 200, "Led_Bars", amntSamples, 0, 0.25f, 32 },
					{ 200, "EQ", amntSamples, -120, 6, amntSamples },
					{ 200, "Spectrum Analyzer", amntSamples, -120, 6, amntSamples }
				};

				std::string view_options[OPTIONS_AMNT];

				for (size_t i = 0; i < OPTIONS_AMNT; i++)
					view_options[i] = dsp_options[i].option_name.c_str();

				const std::string dsp_text = "DSP";
				const std::string dsp_text_id = "##DSP_" + std::to_string(a_Index) + " (" + sound_name + ")" + "##DSP_" + std::to_string(a_Index);
				ImGui::Text("%s", dsp_text.c_str());
				ImGui::SameLine();
				if (ImGui::BeginCombo(dsp_text_id.c_str(), view_options[m_SelectedSP].c_str(), ImGuiComboFlags_PopupAlignLeft))
				{
					for (uint32_t n = 0; n < OPTIONS_AMNT; n++)
					{
						const bool is_selected = n == m_SelectedSP;
						if (ImGui::Selectable(view_options[n].c_str(), is_selected))
							m_SelectedSP = static_cast<fft_option>(n);
					}
					ImGui::EndCombo();
				}

				dsp_analysis_settings& option = dsp_options[m_SelectedSP];

				std::string graph_eq_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_eq_01";
				if (ImPlot::BeginPlot(graph_eq_name.c_str(), ImVec2(ImGui::GetWindowSize().x - 65, option.graph_height), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
				{
					float bytePos;
					channel->GetPos(uaudio::player::TIMEUNIT::TIMEUNIT_POS, bytePos);

					uint32_t bytes = option.numSamples * fmt_chunk.blockAlign;
					sound->PreRead(static_cast<uint32_t>(bytePos), bytes);
					size_t numSSamples = bytes / fmt_chunk.blockAlign;
					if (numSSamples > 0)
					{
						int input_size = static_cast<int>(numSSamples);
						int output_size = (input_size / 2 + 1);

						double* input_buffer = player::utils::ToSample(reinterpret_cast<unsigned char*>(utils::add(data_chunk.data, static_cast<uint32_t>(bytePos))), bytes, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, numSSamples);

						if (m_SelectedSP == Sample || m_SelectedSP == Played_Sample || m_SelectedSP == Scrolling_Sample)
						{
							ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit);
							ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin);
							ImPlot::SetupAxisLimits(ImAxis_Y1, option.min_y, option.max_y, ImPlotCond_Always);
							ImPlot::PlotLine("", input_buffer, static_cast<int>(numSSamples));
						}
						else
						{
							for (size_t i = 0; i < numSSamples; i++)
							{
								double mult = hamming(i, numSSamples);
								input_buffer[i] = mult * input_buffer[i];
							}

							fftw_complex* output_buffer = static_cast<fftw_complex*>(fftw_malloc(output_size * sizeof(fftw_complex)));

							int flags = FFTW_ESTIMATE;
							fftw_plan plan = fftw_plan_dft_r2c_1d(input_size,
								input_buffer,
								output_buffer,
								flags);

							const int NUM_BINS = option.bins;

							double* magn_s = reinterpret_cast<double*>(malloc(NUM_BINS * sizeof(double)));
							double* freq_bins = reinterpret_cast<double*>(malloc(NUM_BINS * sizeof(double)));
							for (size_t i = 0; i < NUM_BINS; i++)
							{
								double d = ((double)fmt_chunk.sampleRate / 2.0) / (double)NUM_BINS * (double)i;
								freq_bins[i] = d;
								magn_s[i] = 1.7E-308;
							}

							fftw_execute(plan);

							for (size_t i = 0; i < numSSamples / 2; ++i)
							{
								auto re = output_buffer[i][0];
								auto im = output_buffer[i][1];
								auto magn = sqrt(re * re + im * im);
								double scaledMagnitude = magn / numSSamples;

								auto freq = i * (fmt_chunk.sampleRate / (double)numSSamples);
								for (size_t j = 0; j < NUM_BINS; ++j)
								{
									if (freq >= freq_bins[j] && freq <= freq_bins[j + 1])
									{
										if (scaledMagnitude > magn_s[j])
											magn_s[j] = scaledMagnitude;
									}
								}
							}

							ImPlot::SetupAxisLimits(ImAxis_Y1, option.min_y, option.max_y, ImPlotCond_Always);
							ImPlot::SetupAxes("Hz", "dB", ImPlotAxisFlags_AutoFit, ImPlotAxisFlags_LockMin);
							if (m_SelectedSP == Led_Bars)
								ImPlot::PlotBars("", freq_bins, magn_s, NUM_BINS, 500);
							else if (m_SelectedSP == EQ)
							{
								for (size_t i = 0; i < NUM_BINS; ++i)
								{
									double d = 10 * (log10(magn_s[i]));
									magn_s[i] = d;
								}

								double ticks[10] = {
									20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000
								};

								const char* tick_labels[10] = {
									"20",
									"50",
									"100",
									"200",
									"500",
									"1k",
									"2k",
									"5k",
									"10k",
									"20k",
								};

								static const double co = -3;
								ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
								ImPlot::SetupAxisTicks(ImAxis_X1, ticks, 10, tick_labels);
								ImPlot::SetNextLineStyle({ 1,1,1,1 });
								ImPlot::PlotInfLines("##3dB", &co, 1, ImPlotInfLinesFlags_Horizontal);
								ImPlot::PlotShaded("", freq_bins, magn_s, NUM_BINS, -INFINITY);
								ImPlot::PlotLine("", freq_bins, magn_s, NUM_BINS, ImPlotLineFlags_SkipNaN, 0);
							}
							else if (m_SelectedSP == Spectrum_Analyzer)
							{
								for (size_t i = 0; i < NUM_BINS; ++i)
								{
									double d = 10 * (log10(magn_s[i]));
									magn_s[i] = d;
								}

								double ticks[10] = {
									20, 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000
								};

								const char* tick_labels[10] = {
									"20",
									"50",
									"100",
									"200",
									"500",
									"1k",
									"2k",
									"5k",
									"10k",
									"20k",
								};

								static const double co = -3;
								ImPlot::SetupAxisTicks(ImAxis_X1, ticks, 10, tick_labels);
								ImPlot::SetNextLineStyle({ 1,1,1,1 });
								ImPlot::PlotInfLines("##3dB", &co, 1, ImPlotInfLinesFlags_Horizontal);
								ImPlot::PlotShaded("", freq_bins, magn_s, NUM_BINS, -INFINITY);
								ImPlot::PlotLine("", freq_bins, magn_s, NUM_BINS, ImPlotLineFlags_SkipNaN, 0);
							}

							free(freq_bins);
							free(magn_s);

							fftw_free(output_buffer);
							fftw_destroy_plan(plan);
						}

						free(input_buffer);
					}
					ImPlot::EndPlot();
				}
			}






















			float height = fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO ? 50.0f : 100.0f;
			float width = 25;
			uint32_t new_pos = pos;
			float ex_width = ImGui::GetWindowSize().x - width - 35;
			std::string graph_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_waveform_graph_01";
			std::string graph_name_2 = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_waveform_graph_02";
			ImGui::BeginPlayPlot(new_pos, final_pos_slider, numSamples, left_samples, graph_name.c_str(), ex_width, height, fmt_chunk.blockAlign);

			ImGui::SameLine();

			size_t steps = 10;

			double left_val = player::utils::GetPeak(channel->m_LastPlayedData, channel->m_LastDataSize, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, steps);
			double right_val = player::utils::GetPeak(channel->m_LastPlayedData, channel->m_LastDataSize, fmt_chunk.bitsPerSample, fmt_chunk.blockAlign, fmt_chunk.numChannels, fmt_chunk.audioFormat, steps, false);

			left_val = ImLerp(channel->m_LVol, left_val, 0.15f);
			right_val = ImLerp(channel->m_RVol, right_val, 0.15f);

			channel->m_LVol = left_val;
			channel->m_RVol = right_val;

			if (!isPlaying)
			{
				channel->m_LastPlayedData = nullptr;
				channel->m_LastDataSize = 0;
			}

			float actual_steps = steps - (steps * 0.15f);

			float meter_width = fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO ? 5.25f : 12.5f;
			std::string meter_name = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_meter_01";
			std::string meter_name_2 = std::string("###Player_" + std::to_string(a_Index)) + "_" + sound_hash_id + "_meter_02";
			ImGui::UvMeter(meter_name.c_str(), ImVec2(meter_width, 90), &left_val, 0, actual_steps, static_cast<int>(actual_steps));
			if (fmt_chunk.numChannels == uaudio::wave_reader::WAVE_CHANNELS_STEREO)
			{
				ImGui::SameLine();
				ImGui::UvMeter(meter_name_2.c_str(), ImVec2(meter_width, 90), &right_val, 0, actual_steps, static_cast<int>(actual_steps));
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
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume from panning %i.", static_cast<int>(a_Index));
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
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume from channel %i.", static_cast<int>(a_Index));
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
				prev_pos = clamp<int32_t>(prev_pos, 0, data_chunk.ChunkSize());
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
				next_pos = clamp<int32_t>(next_pos, 0, data_chunk.ChunkSize());
				channel->SetPos(next_pos);
				channel->Pause();
				channel->PlayRanged(next_pos, static_cast<int>(9600));
			}

			ImGui::SameLine();

			bool isLooping = false;
			presult = channel->IsLooping(isLooping);
			if (UAUDIOPLAYERFAILED(presult))
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve whether channel %i has looping turned on.", static_cast<int>(a_Index));
				return;
			}
			std::string loop_button_text = std::string(RETRY) + "##Loop_Channel_" + std::to_string(a_Index);
			if (ImGui::CheckboxButton(loop_button_text.c_str(), &isLooping, ImVec2(25, 25)))
				channel->SetLooping(isLooping);

			ImGui::Separator();
		}
	}
}