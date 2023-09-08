#include "imgui/AudioImGuiWindow.h"
#include "imgui/tools/MainWindow.h"

#include <cstdint>

#include <imgui/imgui.h>
#include <imgui/implot.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <imgui/backends/imgui_impl_dx9.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

			DeleteWindow();
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
			m_wc = wc;
		}

		void AudioImGuiWindow::Initialize()
		{
			CreateContext();
			CreateImGui();
		}

		void AudioImGuiWindow::ProcessEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam);
		}

		void AudioImGuiWindow::CreateContext()
		{
			if (!CreateDeviceD3D(m_Hwnd))
			{
				CleanupDeviceD3D();
				::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
			}

			::ShowWindow(m_Hwnd, SW_SHOWDEFAULT);
			::UpdateWindow(m_Hwnd);

			// setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImPlot::CreateContext();

			ImGui_ImplWin32_Init(m_Hwnd);
			ImGui_ImplDX9_Init(g_pd3dDevice);
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

			ImGuiStyle& style = ImGui::GetStyle();

			ImGuiStyle m_DarkStyle = ImGui::GetStyle(); const ImVec4 bgColor = ColorFromBytes(37, 37, 38);
			const ImVec4 lightBgColor = ColorFromBytes(82, 82, 85);
			const ImVec4 veryLightBgColor = ColorFromBytes(90, 90, 95);

			const ImVec4 panelColor = ColorFromBytes(51, 51, 55);
			const ImVec4 panelHoverColor = ColorFromBytes(70, 70, 70);
			const ImVec4 panelHoverColor2 = ColorFromBytes(80, 80, 80);
			const ImVec4 panelDark = ColorFromBytes(29, 170, 200);
			const ImVec4 panelActiveColor = ColorFromBytes(0, 119, 200);
			const ImVec4 panelActiverColor = ColorFromBytes(0, 119, 150);

			const ImVec4 textColor = ColorFromBytes(255, 255, 255);
			const ImVec4 textDisabledColor = ColorFromBytes(151, 151, 151);
			const ImVec4 borderColor = ColorFromBytes(78, 78, 78);

			m_DarkStyle.Colors[ImGuiCol_Text] = textColor;
			m_DarkStyle.Colors[ImGuiCol_TextDisabled] = textDisabledColor;
			m_DarkStyle.Colors[ImGuiCol_TextSelectedBg] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_WindowBg] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_ChildBg] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_PopupBg] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_Border] = borderColor;
			m_DarkStyle.Colors[ImGuiCol_BorderShadow] = borderColor;
			m_DarkStyle.Colors[ImGuiCol_FrameBg] = panelColor;
			m_DarkStyle.Colors[ImGuiCol_FrameBgHovered] = panelHoverColor;
			m_DarkStyle.Colors[ImGuiCol_FrameBgActive] = panelHoverColor2;
			m_DarkStyle.Colors[ImGuiCol_TitleBg] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_TitleBgActive] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_TitleBgCollapsed] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_MenuBarBg] = panelColor;
			m_DarkStyle.Colors[ImGuiCol_ScrollbarBg] = panelColor;
			m_DarkStyle.Colors[ImGuiCol_ScrollbarGrab] = lightBgColor;
			m_DarkStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = veryLightBgColor;
			m_DarkStyle.Colors[ImGuiCol_ScrollbarGrabActive] = veryLightBgColor;
			m_DarkStyle.Colors[ImGuiCol_CheckMark] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_SliderGrab] = panelDark;
			m_DarkStyle.Colors[ImGuiCol_SliderGrabActive] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_Button] = panelDark;
			m_DarkStyle.Colors[ImGuiCol_ButtonHovered] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_ButtonActive] = panelActiverColor;
			m_DarkStyle.Colors[ImGuiCol_Header] = panelColor;
			m_DarkStyle.Colors[ImGuiCol_HeaderHovered] = panelHoverColor;
			m_DarkStyle.Colors[ImGuiCol_HeaderActive] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_Separator] = borderColor;
			m_DarkStyle.Colors[ImGuiCol_SeparatorHovered] = borderColor;
			m_DarkStyle.Colors[ImGuiCol_SeparatorActive] = borderColor;
			m_DarkStyle.Colors[ImGuiCol_ResizeGrip] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_ResizeGripHovered] = panelColor;
			m_DarkStyle.Colors[ImGuiCol_ResizeGripActive] = lightBgColor;
			m_DarkStyle.Colors[ImGuiCol_PlotLines] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_PlotLinesHovered] = panelHoverColor;
			m_DarkStyle.Colors[ImGuiCol_PlotHistogram] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_PlotHistogramHovered] = panelHoverColor;
			m_DarkStyle.Colors[ImGuiCol_DragDropTarget] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_NavHighlight] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_DockingPreview] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_Tab] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_TabActive] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_TabUnfocused] = bgColor;
			m_DarkStyle.Colors[ImGuiCol_TabUnfocusedActive] = panelActiveColor;
			m_DarkStyle.Colors[ImGuiCol_TabHovered] = panelHoverColor;

			m_DarkStyle.WindowPadding = ImVec2(10, 10);
			m_DarkStyle.WindowRounding = 10.0f;
			m_DarkStyle.FramePadding = ImVec2(5, 5);
			m_DarkStyle.ItemSpacing = ImVec2(12, 8);
			m_DarkStyle.ItemInnerSpacing = ImVec2(8, 6);
			m_DarkStyle.IndentSpacing = 25.0f;
			m_DarkStyle.ScrollbarSize = 15.0f;
			m_DarkStyle.ScrollbarRounding = 9.0f;
			m_DarkStyle.GrabMinSize = 20.0f; // Make grab a circle
			m_DarkStyle.GrabRounding = 12.0f;
			m_DarkStyle.PopupRounding = 7.f;
			m_DarkStyle.Alpha = 1.0;

			style = m_DarkStyle;
		}

		bool AudioImGuiWindow::CreateDeviceD3D(HWND hWnd)
		{
			if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
				return false;

			// Create the D3DDevice
			ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
			g_d3dpp.Windowed = TRUE;
			g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
			g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
			g_d3dpp.EnableAutoDepthStencil = TRUE;
			g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
			g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
			//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
			if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
				return false;

			return true;
		}

		void AudioImGuiWindow::CleanupDeviceD3D()
		{
			if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
			if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
		}

		void AudioImGuiWindow::ResetDevice()
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}

		void AudioImGuiWindow::Render()
		{
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImVec2 size = ImVec2(1080, 720);
			ImGui::SetNextWindowPos(ImVec2(0, 0));
			ImGui::SetNextWindowSize(size);

			ImGui::PushFont(m_DefaultFont);

			m_MainWindow->SetSize(size);
			m_MainWindow->Update();
			for (auto* tool : m_Tools)
			{
				if (tool->IsFullScreen())
					tool->SetSize(size);
				tool->Update();
			}

			ImGui::PopFont();

			ImGui::EndFrame();
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
			D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
			if (g_pd3dDevice->BeginScene() >= 0)
			{
				ImGui::Render();
				ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
				g_pd3dDevice->EndScene();
			}

			HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

			// Handle loss of D3D9 device
			if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
				ResetDevice();
		}

		void AudioImGuiWindow::DeleteWindow()
		{
			ImGui_ImplWin32_Shutdown();
			ImGui_ImplDX9_Shutdown();
			ImPlot::DestroyContext();
			ImGui::DestroyContext();
			CleanupDeviceD3D();
			::DestroyWindow(m_Hwnd);
			::UnregisterClassW(m_wc.lpszClassName, m_wc.hInstance);
		}

		void AudioImGuiWindow::AddTool(BaseTool& a_Tool)
		{
			m_Tools.push_back(&a_Tool);
		}
	}
}