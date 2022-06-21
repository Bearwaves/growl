#include "sdl2_window.h"
#include "SDL_video.h"

using Growl::SDL2Window;

SDL2Window::SDL2Window(SDL_Window* window)
	: native{window} {}

SDL2Window::~SDL2Window() {
	SDL_DestroyWindow(native);
}
