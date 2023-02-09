#include "imgui/tools/SoundsTool.h"

#include <vector>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <uaudio_wave_reader/WaveChunkData.h>
#include <imgui/imgui_helpers.h>
#include <imgui/implot.h>

#include "audio/player/AudioSystem.h"
#include "audio/player/Sound.h"
#include "audio/utils/Utils.h"
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

		void SoundsTool::ShowBaseChunk(char* a_ChunkId, uaudio::wave_reader::ChunkCollection& chunkCollection)
		{
			ShowValue("Chunk ID: ", a_ChunkId);
			uint32_t size;
			chunkCollection.GetChunkSize(size, a_ChunkId);
			ShowValue("Chunk Size: ", std::to_string(size).c_str());
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

			uint32_t data_chunk_size = 0;

			if (hasFmtChunk && hasDataChunk)
			{
				chunkCollection.GetChunkSize(data_chunk_size, uaudio::wave_reader::DATA_CHUNK_ID);

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

#pragma region duration

			uaudio::wave_reader::FMT_Chunk fmt_chunk;
			chunkCollection.GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);

			ImGui::Text("%s", std::string(
				uaudio::utils::FormatDuration(uaudio::utils::PosToSeconds(0, fmt_chunk.byteRate), true) +
				"/" +
				uaudio::utils::FormatDuration(uaudio::utils::PosToSeconds(data_chunk_size, fmt_chunk.byteRate), true))
				.c_str());

#pragma endregion

#pragma region Chunks

			const std::string chunk_specific_info_text = "Chunk Specific Info" + sound_hash_id + "chunk_specific_info_collapse";
			if (ImGui::CollapsingHeader(chunk_specific_info_text.c_str()))
			{
				ImGui::Indent(IMGUI_INDENT);
				
				size_t num_chunks = 0;
				chunkCollection.GetNumberOfChunks(num_chunks);

				for (size_t i = 0; i < num_chunks; i++)
				{
					uaudio::WaveChunkData* data = nullptr;
					chunkCollection.GetChunkFromData(data, static_cast<uint32_t>(i));

					char chunk_id[uaudio::wave_reader::CHUNK_ID_SIZE + 1];
					char* id = &reinterpret_cast<char*>(data->chunk_id)[0];
					for (int j = 0; j < uaudio::wave_reader::CHUNK_ID_SIZE; j++)
					{
						chunk_id[j] = id[j];
					}
					chunk_id[uaudio::wave_reader::CHUNK_ID_SIZE] = '\0';

					std::string chunk_header = std::string(chunk_id) + sound_hash_id + "chunk_" + std::to_string(i);

					if (uaudio::utils::chunkcmp(uaudio::wave_reader::FMT_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							ShowValue("Audio Format: ", std::to_string(fmt_chunk.audioFormat).c_str());
							ShowValue("Number of Channels: ", std::to_string(fmt_chunk.numChannels).c_str());
							ShowValue("Sample Rate: ", std::to_string(fmt_chunk.sampleRate).c_str());
							ShowValue("Byte Rate: ", std::to_string(fmt_chunk.byteRate).c_str());
							ShowValue("Block Align: ", std::to_string(fmt_chunk.blockAlign).c_str());
							ShowValue("Bits per Sample: ", std::to_string(fmt_chunk.bitsPerSample).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::utils::chunkcmp(uaudio::wave_reader::DATA_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::utils::chunkcmp(uaudio::wave_reader::ACID_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::ACID_Chunk acid_chunk;
							chunkCollection.GetChunkFromData<uaudio::wave_reader::ACID_Chunk>(acid_chunk, uaudio::wave_reader::ACID_CHUNK_ID);
							char type_of_file[256] = {};
							snprintf(type_of_file, 256, "0x%x", acid_chunk.type_of_file);
							ShowValue("Type of File: ", std::string(type_of_file).c_str());
							char root_note[256] = {};
							snprintf(root_note, 256, "0x%x", acid_chunk.root_note);
							ShowValue("Root Note: ", std::string(root_note).c_str());
							char unknown1[256] = {};
							snprintf(unknown1, 256, "0x%x", acid_chunk.unknown1);
							ShowValue("Unknown 1: ", std::string(unknown1).c_str());
							ShowValue("Unknown 2: ", std::to_string(acid_chunk.unknown2).c_str());
							ShowValue("Number of Beats: ", std::to_string(acid_chunk.num_of_beats).c_str());
							ShowValue("Meter Denominator: ", std::to_string(acid_chunk.meter_denominator).c_str());
							ShowValue("Meter Numerator: ", std::to_string(acid_chunk.meter_numerator).c_str());
							ShowValue("Tempo: ", std::to_string(acid_chunk.tempo).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::utils::chunkcmp(uaudio::wave_reader::BEXT_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::BEXT_Chunk bext_chunk;
							chunkCollection.GetChunkFromData<uaudio::wave_reader::BEXT_Chunk>(bext_chunk, uaudio::wave_reader::BEXT_CHUNK_ID);
							ShowValue("Description: ", std::string(bext_chunk.description).c_str());
							ShowValue("Originator: ", std::string(bext_chunk.originator).c_str());
							ShowValue("Originator Reference: ", std::string(bext_chunk.originator_reference).c_str());
							ShowValue("Originator Date: ", std::string(bext_chunk.origination_date).c_str());
							ShowValue("Originator Time: ", std::string(bext_chunk.origination_time).c_str());
							ShowValue("Time Reference Low: ", std::to_string(bext_chunk.time_reference_low).c_str());
							ShowValue("Time Reference High: ", std::to_string(bext_chunk.time_reference_high).c_str());
							ShowValue("Version: ", std::to_string(bext_chunk.version).c_str());
							ShowValue("UMID: ", std::string(reinterpret_cast<const char*>(bext_chunk.umid)).c_str());
							ShowValue("Loudness Value: ", std::to_string(bext_chunk.loudness_value).c_str());
							ShowValue("Loudness Range: ", std::to_string(bext_chunk.loudness_range).c_str());
							ShowValue("Max True Peak Level: ", std::to_string(bext_chunk.max_true_peak_level).c_str());
							ShowValue("Max Momentary Loudness: ", std::to_string(bext_chunk.max_momentary_loudness).c_str());
							ShowValue("Max Short Term Loudness: ", std::to_string(bext_chunk.max_short_term_loudness).c_str());
							ShowValue("Reserved: ", std::string(reinterpret_cast<const char*>(bext_chunk.reserved)).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::utils::chunkcmp(uaudio::wave_reader::FACT_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::FACT_Chunk fact_chunk;
							chunkCollection.GetChunkFromData<uaudio::wave_reader::FACT_Chunk>(fact_chunk, uaudio::wave_reader::FACT_CHUNK_ID);
							ShowValue("Sample Length: ", std::to_string(fact_chunk.sample_length).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::utils::chunkcmp(uaudio::wave_reader::CUE_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::CUE_Chunk cue_chunk;
							chunkCollection.GetChunkFromData<uaudio::wave_reader::CUE_Chunk>(cue_chunk, uaudio::wave_reader::CUE_CHUNK_ID);
							ShowValue("Number of cue points: ", std::to_string(cue_chunk.num_cue_points).c_str());
							for (uint32_t i = 0; i < cue_chunk.num_cue_points; i++)
							{
								const std::string chunk_cue_point_text = chunk_header + "cue_point_" + std::to_string(i);
								if (ImGui::CollapsingHeader(chunk_cue_point_text.c_str()))
								{
									ImGui::Indent(IMGUI_INDENT);
									ShowValue("ID: ", std::to_string(cue_chunk.cue_points[i].id).c_str());
									ShowValue("Position: ", std::to_string(cue_chunk.cue_points[i].position).c_str());
									ShowValue("Data Chunk ID: ", std::string(reinterpret_cast<const char*>(cue_chunk.cue_points[i].data_chunk_id)).c_str());
									ShowValue("Chunk Start: ", std::to_string(cue_chunk.cue_points[i].chunk_start).c_str());
									ShowValue("Block Start: ", std::to_string(cue_chunk.cue_points[i].block_start).c_str());
									ShowValue("Block Offset: ", std::to_string(cue_chunk.cue_points[i].sample_offset).c_str());
									ImGui::Unindent(IMGUI_INDENT);
								}
							}
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::utils::chunkcmp(uaudio::wave_reader::SMPL_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::SMPL_Chunk smpl_chunk;
							chunkCollection.GetChunkFromData<uaudio::wave_reader::SMPL_Chunk>(smpl_chunk, uaudio::wave_reader::SMPL_CHUNK_ID);
							ShowValue("Manufacturer: ", std::to_string(smpl_chunk.manufacturer).c_str());
							ShowValue("Product: ", std::to_string(smpl_chunk.product).c_str());
							ShowValue("Sample Period: ", std::to_string(smpl_chunk.sample_period).c_str());
							ShowValue("MIDI Unity Node: ", std::to_string(smpl_chunk.midi_unity_node).c_str());
							ShowValue("MIDI Pitch Fraction: ", std::to_string(smpl_chunk.midi_pitch_fraction).c_str());
							ShowValue("SMPTE Format: ", std::to_string(smpl_chunk.smpte_format).c_str());
							ShowValue("SMPTE Offset: ", std::to_string(smpl_chunk.smpte_offset).c_str());
							ShowValue("Number of Sample loops: ", std::to_string(smpl_chunk.num_sample_loops).c_str());
							ShowValue("Sampler Data: ", std::to_string(smpl_chunk.sampler_data).c_str());
							for (uint32_t i = 0; i < smpl_chunk.num_sample_loops; i++)
							{
								const std::string chunk_smpl_loop_text = chunk_header + "loop_point_" + std::to_string(i);
								if (ImGui::CollapsingHeader(chunk_smpl_loop_text.c_str()))
								{
									ImGui::Indent(IMGUI_INDENT);
									ShowValue("Cue Point ID: ", std::to_string(smpl_chunk.samples[i].cue_point_id).c_str());
									ShowValue("Type: ", std::to_string(smpl_chunk.samples[i].type).c_str());
									ShowValue("Start: ", std::to_string(smpl_chunk.samples[i].start).c_str());
									ShowValue("End: ", std::to_string(smpl_chunk.samples[i].end).c_str());
									ShowValue("Fraction: ", std::to_string(smpl_chunk.samples[i].fraction).c_str());
									ShowValue("Play Count: ", std::to_string(smpl_chunk.samples[i].play_count).c_str());
									ImGui::Unindent(IMGUI_INDENT);
								}
							}
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							ImGui::Unindent(IMGUI_INDENT);
						}
					}
				}

				ImGui::Unindent(IMGUI_INDENT);
			}
#pragma endregion
		}
	}
}