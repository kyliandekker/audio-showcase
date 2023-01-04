#pragma once

#include <cstdint>
#include <vector>
#include <sdl/SDL.h>

class AudioSDLWindow;

class AudioImGuiWindow
{
public:
	AudioImGuiWindow() = default;
	void SetWindow(AudioSDLWindow* a_Window);
	~AudioImGuiWindow();

	void CreateContext() const;
	void CreateImGui() const;
	void Render();
	void DeleteWindow() const;
	void ProcessEvent(SDL_Event* a_Event);
private:
	AudioSDLWindow* m_Window = nullptr;
	bool m_Enabled = true;
};