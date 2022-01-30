#include <SDL.h>
#include <assert.h>
#include <growl/plugins/sdl2/sdl_system.h>

using Growl::SDL2SystemAPI;

void SDL2SystemAPI::init() {
	assert(SDL_WasInit(SDL_INIT_VIDEO) == 0);

	int flags = SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_ALLOW_HIGHDPI;
	flags |= SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	SDL_Window* win = SDL_CreateWindow(
		"GAME", // creates a window
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 1000, flags);

	running = true;
}

void SDL2SystemAPI::tick() {
	SDL_Event event;
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		running = false;
	}
}

void SDL2SystemAPI::dispose() {
	SDL_Quit();
}

bool SDL2SystemAPI::isRunning() {
	return running;
}
