#pragma once

#include <cstdint>
#include <sdl/SDL.h>
#include <functional>

class AudioSDLWindow
{
public:
	AudioSDLWindow();
	~AudioSDLWindow();

	void Init();
	void RenderWindow();
	SDL_Window* GetWindow() const { return m_Window; }
	SDL_GLContext& GetContext() { return m_glContext; }
	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }
	SDL_Event& GetLastEvent() { return m_Event; }

	std::function<void()> m_Unitialize, m_Initialize, m_UpdateFrame;
	std::function<void(SDL_Event*)> m_EventProcess;
private:
	int32_t CreateSDLWindow();
	int32_t CreateContext();
	int32_t CreateGlad();

	SDL_Window* m_Window = nullptr;
	SDL_GLContext m_glContext = nullptr;
	SDL_Event m_Event;

	bool m_Running = true;
	uint32_t m_Width = 1280, m_Height = 720;
	const char* m_WindowTitle = "Audio Showcase";
};