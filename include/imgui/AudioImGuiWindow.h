#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <imgui/imgui.h>
#include <dx9/DX9Window.h>

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
			void Render();
			void DeleteWindow();
			void AddTool(BaseTool& a_Tool);
			void ResetDevice();
			bool IsEnabled() const;
			void Stop();
			void SetSize(ImVec2 a_Size);

			static void LoggerCallback(logger::Message& message);

			bool m_ShowPopUp = false;
			std::string m_PopUpText;
			std::string m_PopUpTitle;
			std::mutex m_RenderMutex;
		private:
			HWND m_Hwnd = nullptr;
			WNDCLASSEX m_Wc;
			bool m_Enabled = true;

			ImVec2 m_Size;

			dx9::DX9Window m_DX9Window;

			MainWindow* m_MainWindow = nullptr;
			std::vector<BaseTool*> m_Tools;
		};
		extern AudioImGuiWindow window;
	}
}