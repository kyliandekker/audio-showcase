#include "imgui/tools/MasterTool.h"

#include <OleCtl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_helpers.h>
#include <uaudio_wave_reader/WaveChunks.h>
#include <uaudio_wave_reader/WaveReader.h>

#include "imgui/ImguiDefines.h"
#include "audio/storage/SoundsSystem.h"
#include "audio/player/AudioSystem.h"
#include "utils/Logger.h"
#include "audio/player/ChannelHandle.h"
#include "audio/player/AudioChannel.h"

namespace uaudio
{
	namespace imgui
	{
		MasterTool::MasterTool() : BaseTool(0, "Actions", "Master Actions")
		{
			uint32_t buffer_size = 0;
			uaudio::player::UAUDIO_PLAYER_RESULT result = player::audioSystem.GetBufferSize(buffer_size);
			if (result != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK)
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve buffer size: %i", result);
				return;
			}

			for (size_t i = 0; i < m_BufferSizeOptions.size(); i++)
				if (m_BufferSizeOptions[i] == buffer_size)
					m_BufferSizeSelection = i;

			m_ChunkIds.push_back({ uaudio::wave_reader::FMT_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::DATA_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::ACID_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::BEXT_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::FACT_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::CUE_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::SMPL_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::INST_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::LIST_CHUNK_ID, false, false });
		}

		void MasterTool::Render()
		{
			bool paused = false;
			uaudio::player::UAUDIO_PLAYER_RESULT result = player::audioSystem.IsPaused(paused);
			if (result != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK)
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve pause state: %i", result);
				return;
			}
			if (paused)
			{
				if (ImGui::Button(PLAY, ImVec2(50, 50)))
					player::audioSystem.SetPaused(false);
			}
			else
			{
				if (ImGui::Button(PAUSE, ImVec2(50, 50)))
					player::audioSystem.SetPaused(true);
			}
			ImGui::SameLine();
			if (ImGui::Button(STOP, ImVec2(50, 50)))
			{
				player::audioSystem.SetPaused(true);

				size_t size = 0;
				if (uaudio::player::audioSystem.NumChannels(size) != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK)
					return;

				for (uint32_t i = 0; i < size; i++)
				{
					uaudio::player::ChannelHandle handle = i;
					uaudio::player::AudioChannel* channel = nullptr;
					uaudio::player::UAUDIO_PLAYER_RESULT result = uaudio::player::audioSystem.GetChannel(handle, channel);
					if (result != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK || !channel)
					{
						LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve channel: %i", result);
						return;
					}

					channel->SetPos(0);
					channel->Pause();
				}
			}

			float panning = 0;
			result = player::audioSystem.GetPanning(panning);
			if (result != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK)
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve panning: %i", result);
				return;
			}
			const std::string master_panning_text = std::string(PANNING) + " Master Panning (affects all channels)";
			if (ImGui::Knob("Panning##Master_Panning", &panning, -1.0f, 1.0f, ImVec2(50, 50), master_panning_text.c_str(), 0.0f))
				player::audioSystem.SetPanning(panning);

			ImGui::SameLine();
			float volume = 0;
			result = player::audioSystem.GetVolume(volume);
			if (result != uaudio::player::UAUDIO_PLAYER_RESULT::UAUDIO_OK)
			{
				LOGF(uaudio::logger::LOGSEVERITY_WARNING, "Cannot retrieve volume: %i", result);
				return;
			}
			const std::string master_volume_text = std::string(VOLUME_UP) + " Master Volume (affects all channels)";
			if (ImGui::Knob("Volume##Master_Volume", &volume, 0, 1, ImVec2(50, 50), master_volume_text.c_str(), 1.0f))
				player::audioSystem.SetVolume(volume);

			ImGui::Separator();

			const std::string buffer_size_text = "Buffer Size";
			ImGui::Text("%s", buffer_size_text.c_str());
			if (ImGui::BeginCombo("##Buffer_Size", std::string(m_BufferSizeSelection == -1 ? "CHOOSE BUFFER SIZE" : m_BufferSizeTextOptions[m_BufferSizeSelection]).c_str(), ImGuiComboFlags_PopupAlignLeft))
			{
				for (uint32_t n = 0; n < static_cast<uint32_t>(m_BufferSizeTextOptions.size()); n++)
				{
					const bool is_selected = n == m_BufferSizeSelection;
					if (ImGui::Selectable(m_BufferSizeTextOptions[n], is_selected))
					{
						m_BufferSizeSelection = n;
						player::audioSystem.SetBufferSize(m_BufferSizeOptions[n]);
					}
				}
				ImGui::EndCombo();
			}

			ImGui::Separator();

			const std::string add_sound_text = std::string(ADD) + " Load Sound";
			if (ImGui::Button(add_sound_text.c_str()))
				OpenFile();

			ImGui::SameLine();
			bool simd = uaudio::player::audioSystem.simd;
			std::string loop_button_text = std::string(ICON_FA_FAST_FORWARD) + " SIMD##_temp";
			if (ImGui::CheckboxButton(loop_button_text.c_str(), &simd, ImVec2(75, 25)))
				uaudio::player::audioSystem.simd = simd;

			if (ImGui::CollapsingHeader("Extra Options"))
			{
				ImGui::Indent(IMGUI_INDENT);

				const std::string channels_text = "Channels (stereo or mono)";
				ImGui::Text("%s", channels_text.c_str());
				if (ImGui::BeginCombo("##Stereo_Or_Mono", m_ChannelsTextOptions[m_SelectedNumChannels], ImGuiComboFlags_PopupAlignLeft))
				{
					for (uint32_t n = 0; n < static_cast<uint32_t>(m_ChannelsTextOptions.size()); n++)
					{
						const bool is_selected = n == m_SelectedNumChannels;
						if (ImGui::Selectable(m_ChannelsTextOptions[n], is_selected))
							m_SelectedNumChannels = static_cast<uint16_t>(n);
					}
					ImGui::EndCombo();
				}

				const std::string bits_per_sample_text = "Bits per sample";
				ImGui::Text("%s", bits_per_sample_text.c_str());
				if (ImGui::BeginCombo("##Bits_Per_Sample", m_BitsPerSampleTextOptions[m_SelectedBitsPerSample], ImGuiComboFlags_PopupAlignLeft))
				{
					for (uint16_t n = 0; n < static_cast<uint16_t>(m_BitsPerSampleOptions.size()); n++)
					{
						const bool is_selected = n == m_SelectedBitsPerSample;
						if (ImGui::Selectable(m_BitsPerSampleTextOptions[n], is_selected))
							m_SelectedBitsPerSample = n;
					}
					ImGui::EndCombo();
				}

				//const std::string loop_poins_text = "Loop Points";
				//ImGui::Text("%s", loop_poins_text.c_str());
				//if (ImGui::BeginCombo("##Loop_Points", m_LoopPointTextOptions[static_cast<int>(m_WaveConfig.setLoopPoints)], ImGuiComboFlags_PopupAlignLeft))
				//{
				//	for (uint16_t n = 0; n < static_cast<uint16_t>(m_LoopPointTextOptions.size()); n++)
				//	{
				//		const bool is_selected = n == static_cast<int>(m_WaveConfig.setLoopPoints);
				//		if (ImGui::Selectable(m_LoopPointTextOptions[n], is_selected))
				//			m_WaveConfig.setLoopPoints = static_cast<uaudio::LOOP_POINT_SETTING>(n);
				//	}
				//	ImGui::EndCombo();
				//}

				ImGui::Text("%s", "Selected Chunks");
				for (uint32_t i = 0; i < m_ChunkIds.size(); i++)
				{
					if (m_ChunkIds[i].removable)
					{
						ImGui::NewLine();
						ImGui::CheckboxButton(m_ChunkIds[i].chunk_id.c_str(), &m_ChunkIds[i].selected);
						ImGui::SameLine();
						std::string remove_text = std::string(MINUS) + "##RemoveChunk" + std::to_string(i);
						if (ImGui::Button(remove_text.c_str()))
							m_ChunkIds.erase(m_ChunkIds.begin() + i);
					}
					else
					{
						ImGui::CheckboxButton(m_ChunkIds[i].chunk_id.c_str(), &m_ChunkIds[i].selected);
						ImGui::SameLine();
					}
					ImGui::SameLine();
				}
				ImGui::NewLine();
				ImGui::PushItemWidth(100);
				ImGui::InputText("##Add_Chunk", m_SelectedChunkName, uaudio::wave_reader::CHUNK_ID_SIZE + 1);
				ImGui::PopItemWidth();
				ImGui::SameLine();
				if (ImGui::Button("Add"))
				{
					bool canAdd = true;
					chunk_select chunk = { "", false, true };
					chunk.chunk_id = m_SelectedChunkName;
					for (auto& m_ChunkId : m_ChunkIds)
						if (m_ChunkId.chunk_id == chunk.chunk_id)
							canAdd = false;
					if (canAdd)
						m_ChunkIds.push_back(chunk);
				}
				ImGui::Unindent(IMGUI_INDENT);
			}
		}

		/// <summary>
		/// Opens a wav file and adds it to the resources.
		/// </summary>
		void MasterTool::OpenFile()
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

			if (GetOpenFileName(&ofn))
			{
				const auto path = new char[wcslen(ofn.lpstrFile) + 1];
				wsprintfA(path, "%S", ofn.lpstrFile);

				std::string chunks = "";
				for (size_t i = 0; i < m_ChunkIds.size(); i++)
					if (m_ChunkIds[i].selected)
						chunks += m_ChunkIds[i].chunk_id;

				const uaudio::wave_reader::ChunkFilter filters{ chunks.c_str(), chunks.size() / uaudio::wave_reader::CHUNK_ID_SIZE};

				uaudio::storage::soundSystem.AddSound(path, filters);

				delete[] path;
			}
		}
	}
}