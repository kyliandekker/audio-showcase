#include "sdl/SDLWindow.h"

int main(int, char* [])
{
	AudioSDLWindow audioSDLWindow = AudioSDLWindow();

	// Render loop.
	audioSDLWindow.RenderWindow();

	return 0;
}