#include "sdl/AudioSDLWindow.h"
#include "imgui/AudioImGuiWindow.h"
#include "imgui/tools/MasterTool.h"
#include "imgui/tools/SoundsTool.h"

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

	uaudio::imgui::MasterTool masterTool = uaudio::imgui::MasterTool();
	imGuiWindow.AddTool(masterTool);

	uaudio::imgui::SoundsTool soundsTool = uaudio::imgui::SoundsTool();
	imGuiWindow.AddTool(soundsTool);

	imGuiWindow.CreateImGui();

	audioSDLWindow.RenderWindow();

	return 0;
}