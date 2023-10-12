#include "imgui/AudioImGuiWindow.h"

#include <cstdint>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx9.h>
#include <d3d9.h>

#include "imgui/tools/MainWindow.h"
#include "utils/Logger.h"
#include <imgui/implot.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

uaudio::imgui::AudioImGuiWindow uaudio::imgui::window;

namespace uaudio
{
	namespace imgui
	{
		AudioImGuiWindow::AudioImGuiWindow()
		{
			m_MainWindow = new MainWindow(m_Tools);
		}

		AudioImGuiWindow::~AudioImGuiWindow()
		{
			m_Enabled = false;

			delete m_MainWindow;

			m_Tools.clear();
		}

		float GetRGBColor(int color)
		{
			return 1.0f / 255.0f * static_cast<float>(color);
		}

		ImVec4 ColorFromBytes(int r, int g, int b)
		{
			return ImVec4(GetRGBColor(r), GetRGBColor(g), GetRGBColor(b), 1);
		}

		void AudioImGuiWindow::SetHwnd(HWND hwnd, WNDCLASSEX wc)
		{
			m_Hwnd = hwnd;
			m_Wc = wc;
		}

		void AudioImGuiWindow::Initialize()
		{
			logger::logger.m_LoggerCallback = &window.LoggerCallback;
			CreateContext();
			CreateImGui();
		}

		void AudioImGuiWindow::ProcessEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
		}

		void AudioImGuiWindow::CreateContext()
		{
			if (!m_DX9Window.CreateDeviceD3D(m_Hwnd))
			{
				m_DX9Window.CleanupDeviceD3D();
				return;
			}

			::ShowWindow(m_Hwnd, SW_SHOWDEFAULT);
			::UpdateWindow(m_Hwnd);

			// setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImPlot::CreateContext();

			ImGui_ImplWin32_Init(m_Hwnd);
			ImGui_ImplDX9_Init(m_DX9Window.g_pd3dDevice);
		}

		ImFont* m_DefaultFont = nullptr;
		ImFont* m_EditorFont = nullptr;

		void AudioImGuiWindow::CreateImGui() const
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
			(void)io;

			// setup Dear ImGui style
			ImGui::StyleColorsDark();

			ImFont* font1 = io.Fonts->AddFontDefault();
			(void)font1;

			m_DefaultFont = io.Fonts->AddFontFromFileTTF("./resources/font_default.ttf", 14.0f);

			constexpr auto ICON_MIN_FA = 0xf000;
			constexpr auto ICON_MAX_FA = 0xf2e0;
			constexpr ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
			ImFontConfig icons_config;
			icons_config.MergeMode = true;
			icons_config.PixelSnapH = true;
			m_EditorFont = io.Fonts->AddFontFromFileTTF("./resources/fontawesome.ttf", 14.0f, &icons_config, icons_ranges);
			io.Fonts->Build();

			ImGui::StyleColorsDark();

			ImVec4* colors = ImGui::GetStyle().Colors;
			colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
			colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
			colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
			colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
			colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
			colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
			colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 0.66f, 0.18f, 0.54f);
			colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
			colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
			colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
			colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
			colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
			colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
			colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
			colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
			colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
			colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
			colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
			colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
			colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
			colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
			colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
			colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
			colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
			colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
			colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
			colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
			colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
			colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
			colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
			colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
			colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
			colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
			colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
			colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
			colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
			colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
			colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
			colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
			colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
			colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

			ImGuiStyle& style = ImGui::GetStyle();
			style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
			style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
			style.IndentSpacing = 25;
			style.ScrollbarSize = 15;
			style.GrabMinSize = 10;
			style.WindowBorderSize = 1;
			style.ChildBorderSize = 1;
			style.PopupBorderSize = 1;
			style.FrameBorderSize = 1;
			style.TabBorderSize = 1;
			style.WindowRounding = 7;
			style.ChildRounding = 4;
			style.FrameRounding = 3;
			style.PopupRounding = 4;
			style.ScrollbarRounding = 9;
			style.GrabRounding = 3;
			style.LogSliderDeadzone = 4;
			style.TabRounding = 4;

			ImPlotStyle& pStyle = ImPlot::GetStyle();

			colors = pStyle.Colors;
			colors[ImPlotCol_Line] = ImVec4(1.00f, 0.66f, 0.18f, 0.24f);

			pStyle.PlotPadding = ImVec2(0, 0);
			pStyle.LabelPadding = ImVec2(0, 0);
			pStyle.LegendPadding = ImVec2(0, 0);
			pStyle.LegendInnerPadding = ImVec2(0, 0);
			pStyle.LegendSpacing = ImVec2(0, 0);
			pStyle.MousePosPadding = ImVec2(0, 0);
			pStyle.AnnotationPadding = ImVec2(0, 0);
			pStyle.FitPadding = ImVec2(0, 0);
			pStyle.PlotDefaultSize = ImVec2(0, 0);
			pStyle.PlotMinSize = ImVec2(0, 0);
		}

		void AudioImGuiWindow::ResetDevice()
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			m_DX9Window.ResetDevice();
			ImGui_ImplDX9_CreateDeviceObjects();
		}

		bool AudioImGuiWindow::IsEnabled() const
		{
			return m_Enabled;
		}

		void AudioImGuiWindow::Stop()
		{
			uaudio::imgui::window.m_RenderMutex.lock();
			m_Enabled = false;
			uaudio::imgui::window.m_RenderMutex.unlock();
		}

		void AudioImGuiWindow::SetSize(ImVec2 a_Size)
		{
			m_Size = a_Size;
		}

		void AudioImGuiWindow::Render()
		{
			if (!m_Enabled)
				return;

			m_RenderMutex.lock();

			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImVec2 size = m_Size;
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(size);

			ImGui::PushFont(m_DefaultFont);

			m_MainWindow->SetSize(size);
			m_MainWindow->Update();
			
			if (m_ShowPopUp)
			{
				ImGui::SetNextWindowSize(ImVec2(size.x / 2, size.y / 3));
				ImGui::OpenPopup(m_PopUpTitle.c_str());

				if (ImGui::BeginPopupModal(m_PopUpTitle.c_str(), &m_ShowPopUp))
				{
					ImGui::PushTextWrapPos(size.x / 2);
					ImGui::Text(m_PopUpText.c_str());
					if (ImGui::Button("Close"))
					{
						m_ShowPopUp = false;
						ImGui::CloseCurrentPopup();
					}
					ImGui::EndPopup();
				}
			}

			for (auto* tool : m_Tools)
			{
				if (tool->IsFullScreen())
					tool->SetSize(size);
				tool->Update();
			}

			ImGui::PopFont();

			ImGui::EndFrame();
			m_DX9Window.SetRenderState();
			m_DX9Window.Clear();
			if (m_DX9Window.BeginScene())
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				m_DX9Window.EndScene();
			}

			HRESULT result = m_DX9Window.g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && m_DX9Window.g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();

			m_RenderMutex.unlock();
		}

		void AudioImGuiWindow::DeleteWindow()
		{
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImPlot::DestroyContext();
			ImGui::DestroyContext();
			m_DX9Window.CleanupDeviceD3D();
		}

		void AudioImGuiWindow::AddTool(BaseTool& a_Tool)
		{
			m_Tools.push_back(&a_Tool);
		}

		void AudioImGuiWindow::LoggerCallback(logger::Message& message)
		{
			if (message.severity != logger::LOGSEVERITY_ERROR && message.severity != logger::LOGSEVERITY_ASSERT)
				return;

			window.m_PopUpText = message.message;
			window.m_PopUpTitle = (message.severity == logger::LOGSEVERITY_ERROR) ? "Error" : "Critical Error";
			window.m_ShowPopUp = true;
		}
	}
}