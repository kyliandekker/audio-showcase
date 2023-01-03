#include "sdl/SDLWindow.h"

#include <glad/glad.h>

AudioSDLWindow::AudioSDLWindow()
{
	CreateSDLWindow();
	CreateContext();
	CreateGlad();
	if (m_Initialize != nullptr)
		m_Initialize();
}

AudioSDLWindow::~AudioSDLWindow()
{
	m_Running = false;

	if (m_Unitialize != nullptr)
		m_Unitialize();

	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_Window);
	SDL_Quit();
}

void AudioSDLWindow::RenderWindow()
{
	glViewport(0, 0, m_Width, m_Height);

	while (m_Running)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		glClearColor(0, 0, 0, 255);

		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (m_EventProcess != nullptr)
				m_EventProcess(&event);

			switch (event.type)
			{
				case SDL_QUIT:
				{
					m_Running = false;
					break;
				}
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
					case SDL_WINDOWEVENT_RESIZED:
					{
						m_Width = event.window.data1;
						m_Height = event.window.data2;
						glViewport(0, 0, m_Width, m_Height);
						break;
					}
					default:
					{
						break;
					}
					}
					break;
				}
				case SDL_KEYDOWN:
				{
					switch (event.key.keysym.sym)
					{
					case SDLK_ESCAPE:
					{
						m_Running = false;
						break;
					}
					default:
					{
						break;
					}
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}

		if (m_UpdateFrame != nullptr)
			m_UpdateFrame();

		SDL_GL_SwapWindow(m_Window);
	}
	printf("Main loop ended.");
}

int32_t AudioSDLWindow::CreateSDLWindow()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		printf("%s.", SDL_GetError());
		return -1;
	}

	m_Window = SDL_CreateWindow(
		m_WindowTitle,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		m_Width,
		m_Height,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_OPENGL);

	SDL_SetWindowMinimumSize(m_Window, 500, 300);

	printf("Created SDL Window.\n");

	return 0;
}

int32_t AudioSDLWindow::CreateContext()
{
	m_glContext = SDL_GL_CreateContext(m_Window);
	SDL_GL_MakeCurrent(m_Window, m_glContext);

	SDL_GL_SetSwapInterval(1);

	printf("Created SDL Context.\n");

	return 0;
}

int32_t AudioSDLWindow::CreateGlad()
{
	if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
	{
		printf("Couldn't initialize GLAD.");
		return -1;
	}
	printf("Initialized GLAD.");
	return 0;
}