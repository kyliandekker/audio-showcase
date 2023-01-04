#include "sdl/AudioSDLWindow.h"
#include "imgui/AudioImGuiWindow.h"

int main(int, char* [])
{
	AudioSDLWindow audioSDLWindow = AudioSDLWindow();

	AudioImGuiWindow imGuiWindow = AudioImGuiWindow();
	imGuiWindow.SetWindow(&audioSDLWindow);

	audioSDLWindow.m_Initialize = std::bind(&AudioImGuiWindow::CreateContext, &imGuiWindow);
	audioSDLWindow.m_Unitialize = std::bind(&AudioImGuiWindow::DeleteWindow, &imGuiWindow);
	audioSDLWindow.m_UpdateFrame = std::bind(&AudioImGuiWindow::Render, &imGuiWindow);
	audioSDLWindow.m_EventProcess = std::bind(&AudioImGuiWindow::ProcessEvent, &imGuiWindow, &audioSDLWindow.GetLastEvent());
	audioSDLWindow.Init();

	imGuiWindow.CreateImGui();

	audioSDLWindow.RenderWindow();

	return 0;
}