#pragma once

#include <cstdint>
#include <sdl/SDL.h>
#include <functional>

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();

	void RenderWindow();
private:
	int32_t CreateSDLWindow();
	int32_t CreateContext();
	int32_t CreateGlad();

	SDL_Window* m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;

	bool m_Running = true;
	int32_t m_Width = 1280, m_Height = 720;
	const char* m_WindowTitle = "Audio Showcase";

	std::function<void()> m_Unitialize, m_Initialize, m_UpdateFrame;
	std::function<void(SDL_Event*)> m_EventProcess;
};