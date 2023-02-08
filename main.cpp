#include "sdl/AudioSDLWindow.h"
#include "imgui/AudioImGuiWindow.h"

int main(int, char* [])
{
	uaudio::sdl::AudioSDLWindow audioSDLWindow = uaudio::sdl::AudioSDLWindow();

	uaudio::imgui::AudioImGuiWindow imGuiWindow = uaudio::imgui::AudioImGuiWindow();
	imGuiWindow.SetWindow(&audioSDLWindow);

	audioSDLWindow.m_Initialize = std::bind(&uaudio::imgui::AudioImGuiWindow::CreateContext, &imGuiWindow);
	audioSDLWindow.m_Unitialize = std::bind(&uaudio::imgui::AudioImGuiWindow::DeleteWindow, &imGuiWindow);
	audioSDLWindow.m_UpdateFrame = std::bind(&uaudio::imgui::AudioImGuiWindow::Render, &imGuiWindow);
	audioSDLWindow.m_EventProcess = std::bind(&uaudio::imgui::AudioImGuiWindow::ProcessEvent, &imGuiWindow, &audioSDLWindow.GetLastEvent());
	audioSDLWindow.Init();

	imGuiWindow.CreateImGui();

	audioSDLWindow.RenderWindow();

	return 0;
}