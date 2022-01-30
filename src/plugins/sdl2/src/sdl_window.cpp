#include <growl/plugins/sdl2/sdl_window.h>

using Growl::SDL2Window;

SDL2Window::SDL2Window(SDL_Window* window) : native{window} {}
