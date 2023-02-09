#include "imgui/tools/MasterTool.h"

#include <OleCtl.h>
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>
#include <imgui/imgui_helpers.h>
#include <uaudio_wave_reader/WaveChunks.h>

#include "imgui/ImguiDefines.h"
#include <audio/player/AudioSystem.h>

namespace uaudio
{
	namespace imgui
	{
		MasterTool::MasterTool() : BaseTool(0, "Actions", "Master Actions")
		{
			//uaudio::BUFFERSIZE buffer_size;
			//m_AudioSystem.GetBufferSize(buffer_size);
			//for (int i = 0; i < m_BufferSizeOptions.size(); i++)
			//	if (m_BufferSizeOptions[i] == buffer_size)
			//		m_BufferSizeSelection = i;

			//m_WaveConfig.bitsPerSample = uaudio::UAUDIO_DEFAULT_BITS_PER_SAMPLE;
			//for (uint32_t i = 0; i < m_BitsPerSampleOptions.size(); i++)
			//	if (m_WaveConfig.bitsPerSample == m_BitsPerSampleOptions[i])
			//		m_SelectedBitsPerSample = i;

			m_ChunkIds.push_back({ uaudio::wave_reader::FMT_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::DATA_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::ACID_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::BEXT_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::FACT_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::CUE_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::SMPL_CHUNK_ID, false, false });
			m_ChunkIds.push_back({ uaudio::wave_reader::INST_CHUNK_ID, false, false });
		}

		void MasterTool::Render()
		{
			bool playback = false;
			// m_AudioSystem.GetPlayback(playback);
			if (playback)
			{
				if (ImGui::Button(PAUSE, ImVec2(50, 50)))
				{
					// m_AudioSystem.SetPlayback(false);
				}
			}
			else
			{
				if (ImGui::Button(PLAY, ImVec2(50, 50)))
				{
					// m_AudioSystem.SetPlayback(true);
				}
			}
			ImGui::SameLine();
			//if (ImGui::Button(STOP, ImVec2(50, 50)))
			//{
			//    m_AudioSystem.SetPlayback(false);
			//    for (int32_t i = 0; i < static_cast<int32_t>(m_AudioSystem.ChannelSize()); i++)
			//        m_AudioSystem.GetChannel(i)->SetPos(0);
			//}

			float panning = 0;
			// m_AudioSystem.GetMasterPanning(panning);
			const std::string master_panning_text = std::string(PANNING) + " Master Panning (affects all channels)";
			if (ImGui::Knob("Panning##Master_Panning", &panning, -1.0f, 1.0f, ImVec2(50, 50), master_panning_text.c_str(), 0.0f))
			{
				// m_AudioSystem.SetMasterPanning(panning);
			}

			ImGui::SameLine();
			float volume = 0;
			// m_AudioSystem.GetMasterVolume(volume);
			const std::string master_volume_text = std::string(VOLUME_UP) + " Master Volume (affects all channels)";
			if (ImGui::Knob("Volume##Master_Volume", &volume, 0, 1, ImVec2(50, 50), master_volume_text.c_str(), 1.0f))
			{
				// m_AudioSystem.SetMasterVolume(volume);
			}

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
						// m_AudioSystem.SetBufferSize(m_BufferSizeOptions[n]);
					}
				}
				ImGui::EndCombo();
			}

			const std::string add_sound_text = std::string(ADD) + " Load Sound";
			if (ImGui::Button(add_sound_text.c_str()))
				OpenFile();

			if (ImGui::CollapsingHeader("Extra Options"))
			{
				ImGui::Indent(IMGUI_INDENT);

				//const std::string channels_text = "Channels (stereo or mono)";
				//ImGui::Text("%s", channels_text.c_str());
				//if (ImGui::BeginCombo("##Stereo_Or_Mono", m_ChannelsTextOptions[m_WaveConfig.numChannels], ImGuiComboFlags_PopupAlignLeft))
				//{
				//	for (uint32_t n = 0; n < static_cast<uint32_t>(m_ChannelsTextOptions.size()); n++)
				//	{
				//		const bool is_selected = n == m_WaveConfig.numChannels;
				//		if (ImGui::Selectable(m_ChannelsTextOptions[n], is_selected))
				//			m_WaveConfig.numChannels = static_cast<uint16_t>(n);
				//	}
				//	ImGui::EndCombo();
				//}

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

				uaudio::player::audioSystem.AddSound(path);
				delete[] path;
			}
		}
	}
}