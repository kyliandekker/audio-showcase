#pragma once

#include <cstdint>
#include <vector>
#include <d3d9.h>

namespace uaudio
{
	namespace logger
	{
		struct Message;
	}
	namespace imgui
	{
		class BaseTool;
		class MainWindow;

		class AudioImGuiWindow
		{
		public:
			AudioImGuiWindow();
			~AudioImGuiWindow();

			void SetHwnd(HWND hwnd, WNDCLASSEX wc);
			void Initialize();
			void ProcessEvents(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
			void CreateContext();
			void CreateImGui() const;
			bool CreateDeviceD3D(HWND hWnd);
			void CleanupDeviceD3D();
			void ResetDevice();
			void Render();
			void DeleteWindow();
			void AddTool(BaseTool& a_Tool);
		private:
			HWND m_Hwnd;
			WNDCLASSEX m_wc;
			bool m_Enabled = true;

			MainWindow* m_MainWindow = nullptr;
			std::vector<BaseTool*> m_Tools;

			LPDIRECT3D9 g_pD3D = NULL;
			LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
			D3DPRESENT_PARAMETERS g_d3dpp = {};
		};
	}
}