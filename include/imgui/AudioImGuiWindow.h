#pragma once

#include <cstdint>
#include <vector>
#include <sdl/SDL.h>

namespace uaudio
{
	namespace sdl
	{
		class AudioSDLWindow;
	}
	namespace imgui
	{
		class BaseTool;
		class MainWindow;

		class AudioImGuiWindow
		{
		public:
			AudioImGuiWindow();
			void SetWindow(sdl::AudioSDLWindow* a_Window);
			~AudioImGuiWindow();

			void CreateContext() const;
			void CreateImGui() const;
			void Render();
			void DeleteWindow() const;
			void ProcessEvent(SDL_Event* a_Event);
			void AddTool(BaseTool* a_Tool);
		private:
			sdl::AudioSDLWindow* m_Window = nullptr;
			bool m_Enabled = true;

			MainWindow* m_MainWindow = nullptr;
			std::vector<BaseTool*> m_Tools;
		};
	}
}