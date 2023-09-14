#include "imgui/tools/SoundsTool.h"

#include <vector>
#include <OleCtl.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/WaveReader.h>
#include <uaudio_wave_reader/ChunkCollection.h>
#include <imgui/imgui_helpers.h>
#include <imgui/implot.h>

#include "audio/storage/SoundsSystem.h"
#include "audio/storage/Sound.h"
#include "audio/player/utils.h"
#include "imgui/ImguiDefines.h"
#include "audio/utils/int24_t.h"
#include "audio/utils/uint24_t.h"
#include "audio/player/AudioSystem.h"
#include "audio/player/ChannelHandle.h"
#include "utils/Logger.h"

namespace uaudio
{
	namespace imgui
	{
		SoundsTool::SoundsTool() : BaseTool(0, "Sounds", "Sounds")
		{
		}

		void SoundsTool::Render()
		{
			std::vector<uaudio::storage::Sound*> sounds = uaudio::storage::soundSystem.GetSounds();
			for (int32_t i = static_cast<int32_t>(sounds.size() - 1); i > -1; i--)
			{
				RenderSound(*sounds[i]);
			}
		}

		void SoundsTool::ShowBaseChunk(char* a_ChunkId, uaudio::wave_reader::ChunkCollection& chunkCollection)
		{
			ShowValue("Chunk ID: ", a_ChunkId);
			uint32_t size;
			chunkCollection.GetChunkSize(size, a_ChunkId);
			ShowValue("Chunk Size: ", std::to_string(size).c_str());
		}

		void SoundsTool::RenderSound(uaudio::storage::Sound& a_Sound)
		{
			std::string sound_hash_id = "##sound_" + std::to_string(a_Sound.m_Hash) + "_";

			std::string sound_header = std::string(MUSIC) + " \"" + a_Sound.m_Name + "\"" + sound_hash_id + "sound_collapse";
			if (!ImGui::CollapsingHeader(sound_header.c_str()))
				return;

			ImGui::Indent(IMGUI_INDENT);
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
					uaudio::player::ChannelHandle handle;
					uaudio::player::audioSystem.Play(a_Sound, handle);
				}
				ImGui::SameLine();
			}

			std::string remove_sound_text = std::string(MINUS) + " Unload" + sound_hash_id + "unload_sound_button";
			if (ImGui::Button(remove_sound_text.c_str()))
			{
				uaudio::player::audioSystem.m_Update.lock();
				uaudio::player::audioSystem.RemoveSound(a_Sound);
				uaudio::storage::soundSystem.UnloadSound(a_Sound.m_Hash);
				uaudio::player::audioSystem.m_Update.unlock();
				return;
			}

			ImGui::SameLine();
			std::string save_sound_text = std::string(SAVE) + " Save" + sound_hash_id + "save_sound_button";
			if (ImGui::Button(save_sound_text.c_str()))
			{
				SaveFile(chunkCollection);
			}

#pragma region duration

			if (hasFmtChunk && hasDataChunk)
			{
				uaudio::wave_reader::FMT_Chunk fmt_chunk;
				uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = chunkCollection.GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
				if (UAUDIOWAVEREADERFAILED(result))
				{
					LOGF(logger::LOGSEVERITY_WARNING, "Tried to read from sound %s, but it has no fmt chunk.", m_Name.c_str());
					return;
				}

				ImGui::Text("%s", std::string(
					uaudio::player::utils::FormatDuration(uaudio::player::utils::PosToSeconds(0, fmt_chunk.byteRate), true) +
					"/" +
					uaudio::player::utils::FormatDuration(uaudio::player::utils::PosToSeconds(data_chunk_size, fmt_chunk.byteRate), true))
					.c_str());
			}

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
					uaudio::wave_reader::ChunkHeader* data = nullptr;
					uaudio::wave_reader::UAUDIO_WAVE_READER_RESULT result = chunkCollection.GetChunkFromData(data, static_cast<uint32_t>(i));
					if (UAUDIOWAVEREADERFAILED(result))
					{
						LOGF(logger::LOGSEVERITY_WARNING, "Tried to read from sound %s, but something went wrong.", m_Name.c_str());
						continue;
					}

					char chunk_id[uaudio::wave_reader::CHUNK_ID_SIZE + 1];
					memcpy(&chunk_id, data->chunk_id, uaudio::wave_reader::CHUNK_ID_SIZE);
					chunk_id[uaudio::wave_reader::CHUNK_ID_SIZE] = '\0';

					std::string chunk_header = std::string(chunk_id) + sound_hash_id + "chunk_" + std::to_string(i);

					if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::FMT_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::FMT_Chunk fmt_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::FMT_Chunk>(fmt_chunk, uaudio::wave_reader::FMT_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read fmt chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

							ShowValue("Audio Format: ", std::to_string(fmt_chunk.audioFormat).c_str());
							ShowValue("Number of Channels: ", std::to_string(fmt_chunk.numChannels).c_str());
							ShowValue("Sample Rate: ", std::to_string(fmt_chunk.sampleRate).c_str());
							ShowValue("Byte Rate: ", std::to_string(fmt_chunk.byteRate).c_str());
							ShowValue("Block Align: ", std::to_string(fmt_chunk.blockAlign).c_str());
							ShowValue("Bits per Sample: ", std::to_string(fmt_chunk.bitsPerSample).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::DATA_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::ACID_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::ACID_Chunk acid_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::ACID_Chunk>(acid_chunk, uaudio::wave_reader::ACID_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read acid chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

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
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::BEXT_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::BEXT_Chunk bext_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::BEXT_Chunk>(bext_chunk, uaudio::wave_reader::BEXT_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read bext chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

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
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::FACT_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::FACT_Chunk fact_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::FACT_Chunk>(fact_chunk, uaudio::wave_reader::FACT_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read fact chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

							ShowValue("Sample Length: ", std::to_string(fact_chunk.sample_length).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::CUE_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::CUE_Chunk cue_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::CUE_Chunk>(cue_chunk, uaudio::wave_reader::CUE_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read cue chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

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
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::SMPL_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::SMPL_Chunk smpl_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::SMPL_Chunk>(smpl_chunk, uaudio::wave_reader::SMPL_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read smpl chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

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
								const std::string chunk_smpl_loop_text = "Cue Point " + std::to_string(i) + sound_hash_id + "loop_point_" + std::to_string(i);
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
					else if (uaudio::player::utils::chunkcmp(uaudio::wave_reader::INST_CHUNK_ID, &reinterpret_cast<char*>(data->chunk_id)[0]))
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							uaudio::wave_reader::INST_Chunk smpl_chunk;
							result = chunkCollection.GetChunkFromData<uaudio::wave_reader::INST_Chunk>(smpl_chunk, uaudio::wave_reader::INST_CHUNK_ID);
							if (UAUDIOWAVEREADERFAILED(result))
							{
								LOGF(logger::LOGSEVERITY_WARNING, "Tried to read inst chunk from sound %s but something went wrong.", m_Name.c_str());
								return;
							}

							ShowValue("Unshifted Note: ", std::to_string(smpl_chunk.unshiftedNote).c_str());
							ShowValue("Fine Tune: ", std::to_string(smpl_chunk.fineTune).c_str());
							ShowValue("Gain: ", std::to_string(smpl_chunk.gain).c_str());
							ShowValue("Low Note: ", std::to_string(smpl_chunk.lowNote).c_str());
							ShowValue("High Note: ", std::to_string(smpl_chunk.highNote).c_str());
							ShowValue("Low Velocity: ", std::to_string(smpl_chunk.lowVelocity).c_str());
							ShowValue("High Velocity: ", std::to_string(smpl_chunk.highVelocity).c_str());
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
					else
					{
						if (ImGui::CollapsingHeader(chunk_header.c_str()))
						{
							ImGui::Indent(IMGUI_INDENT);
							ShowBaseChunk(chunk_id, chunkCollection);

							std::string view_as_header = "View Unknown Data" + sound_hash_id + "chunk_" + std::to_string(i);
							if (ImGui::CollapsingHeader(view_as_header.c_str()))
							{
								ImGui::Indent(IMGUI_INDENT);
								static uint32_t view_option = 0;
								std::string view_options[14] =
								{
									"CHOOSE OPTION",
									"int8",
									"uint8",
									"int16",
									"uint16",
									"int24",
									"uint24",
									"int32",
									"uint32",
									"int64",
									"uint64",
									"char",
									"readable char",
									"string"
								};

								const std::string view_as_text = "View as";
								const std::string view_as_text_id = "##View_As_" + std::to_string(i);
								ImGui::Text("%s", view_as_text.c_str());
								if (ImGui::BeginCombo(view_as_text_id.c_str(), view_options[view_option].c_str(), ImGuiComboFlags_PopupAlignLeft))
								{
									for (uint32_t n = 0; n < static_cast<uint32_t>(ARRAYSIZE(view_options)); n++)
									{
										const bool is_selected = n == view_option;
										if (ImGui::Selectable(view_options[n].c_str(), is_selected))
											view_option = n;
									}
									ImGui::EndCombo();
								}

								const std::string endianness_text = "Endianness";
								const std::string endianness_text_id = "##Endianness" + std::to_string(i);
								static uint32_t endianness_option = 0;
								std::string endianness_options[2] =
								{
									"Little Endian",
									"Big Endian",
								};

								ImGui::Text("%s", endianness_text.c_str());
								if (ImGui::BeginCombo(endianness_text_id.c_str(), endianness_options[endianness_option].c_str(), ImGuiComboFlags_PopupAlignLeft))
								{
									for (uint32_t n = 0; n < static_cast<uint32_t>(ARRAYSIZE(endianness_options)); n++)
									{
										const bool is_selected = n == endianness_option;
										if (ImGui::Selectable(endianness_options[n].c_str(), is_selected))
											endianness_option = n;
									}
									ImGui::EndCombo();
								}

								switch (view_option)
								{
									case 1:
									{
										ViewAs<int8_t>(data, endianness_option);
										break;
									}
									case 2:
									{
										ViewAs<uint8_t>(data, endianness_option);
										break;
									}
									case 3:
									{
										ViewAs<int16_t>(data, endianness_option);
										break;
									}
									case 4:
									{
										ViewAs<uint16_t>(data, endianness_option);
										break;
									}
									case 5:
									{
										ViewAs<int24_t>(data, endianness_option);
										break;
									}
									case 6:
									{
										ViewAs<uint24_t>(data, endianness_option);
										break;
									}
									case 7:
									{
										ViewAs<int32_t>(data, endianness_option);
										break;
									}
									case 8:
									{
										ViewAs<uint32_t>(data, endianness_option);
										break;
									}
									case 9:
									{
										ViewAs<int64_t>(data, endianness_option);
										break;
									}
									case 10:
									{
										ViewAs<uint64_t>(data, endianness_option);
										break;
									}
									case 11:
									{
										ViewAs<char>(data, endianness_option);
										break;
									}
									case 12:
									{
										ViewAsChar(data, endianness_option);
										break;
									}
									case 13:
									{
										ViewAsString(data, endianness_option);
										break;
									}
									default:
									{
										break;
									}
								}
								ImGui::Unindent(IMGUI_INDENT);
							}
							ImGui::Unindent(IMGUI_INDENT);
						}
					}
				}

				ImGui::Unindent(IMGUI_INDENT);
			}
#pragma endregion
			ImGui::Unindent(IMGUI_INDENT);
		}

		void SoundsTool::SaveFile(uaudio::wave_reader::ChunkCollection& chunkCollection)
		{
			OPENFILENAME ofn;
			TCHAR sz_file[260] = { 0 };

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = sz_file;
			ofn.nMaxFile = sizeof(sz_file);
			ofn.lpstrFilter = L"WAV Files (*.wav;*.wave)\0*.wav;*.wave";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = nullptr;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = nullptr;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetSaveFileName(&ofn))
			{
				const auto path = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(path, "%S", ofn.lpstrFile);

				uaudio::wave_reader::WaveReader::SaveWave(path, chunkCollection);
				delete[] path;
			}
		}
		
		void reverseBytes(unsigned char* start, int size)
		{
			unsigned char* lo = start;
			unsigned char* hi = start + size - 1;
			unsigned char swap;
			while (lo < hi)
			{
				swap = *lo;
				*lo++ = *hi;
				*hi-- = swap;
			}
		}

		template <class T>
		void SoundsTool::ViewAs(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness)
		{
			for (size_t i = 0; i < a_ChunkHeader->chunkSize / sizeof(T); i++)
			{
				unsigned char* ptr = reinterpret_cast<unsigned char*>(utils::add(a_ChunkHeader, sizeof(uaudio::wave_reader::ChunkHeader)));
				ptr = reinterpret_cast<unsigned char*>(utils::add(ptr, i * sizeof(T)));
				unsigned char complete[sizeof(T)];
				for (size_t i = 0; i < sizeof(T); i++)
					complete[i] = ptr[i];

				if (a_Endianness == 1)
					reverseBytes(complete, sizeof(T));
				T cast;
				std::memcpy(&cast, complete, sizeof(T));
				ShowValue(std::to_string(i).c_str(), std::to_string(cast).c_str());
			}
		}

		void SoundsTool::ViewAsChar(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness)
		{
			for (size_t i = 0; i < a_ChunkHeader->chunkSize; i++)
			{
				unsigned char* ptr = reinterpret_cast<unsigned char*>(utils::add(a_ChunkHeader, sizeof(uaudio::wave_reader::ChunkHeader)));
				ptr = reinterpret_cast<unsigned char*>(utils::add(ptr, i));
				unsigned char complete[2];
				complete[0] = ptr[0];
				complete[1] = '\0';

				ShowValue(std::to_string(i).c_str(), std::string(reinterpret_cast<char*>(complete)).c_str());
			}
		}

		void SoundsTool::ViewAsString(uaudio::wave_reader::ChunkHeader* a_ChunkHeader, uint32_t a_Endianness)
		{
			unsigned char* complete = reinterpret_cast<unsigned char*>(malloc(a_ChunkHeader->chunkSize - sizeof(uaudio::wave_reader::ChunkHeader)));
			if (complete != nullptr)
			{
				memcpy(complete, utils::add(a_ChunkHeader, sizeof(uaudio::wave_reader::ChunkHeader)), a_ChunkHeader->chunkSize - sizeof(uaudio::wave_reader::ChunkHeader));
				ShowValue("Text", std::string(reinterpret_cast<char*>(complete)).c_str());
				free(complete);
			}
		}
	}
}