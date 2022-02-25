#include "sdl_system.h"
#include "sdl_window.h"
#include <assert.h>

using Growl::SDL2SystemAPI;
using Growl::Window;

void SDL2SystemAPI::init() {
	assert(SDL_WasInit(SDL_INIT_VIDEO) == 0);
	SDL_LogSetPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LOG_PRIORITY_INFO);

	running = true;
}

std::unique_ptr<Window>
SDL2SystemAPI::createWindow(const WindowConfig& config) {
	int flags = SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_ALLOW_HIGHDPI;
	flags |= SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;

	SDL_Window* win = SDL_CreateWindow(
		config.getTitle().c_str(),
		config.isCentred() ? SDL_WINDOWPOS_CENTERED : 0,
		config.isCentred() ? SDL_WINDOWPOS_CENTERED : 0, config.getWidth(),
		config.getHeight(), flags);

	return std::make_unique<SDL2Window>(win);
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

void SDL2SystemAPI::setLogLevel(LogLevel logLevel) {
	SDL_LogSetPriority(SDL_LOG_CATEGORY_CUSTOM, getLogPriority(logLevel));
}

void SDL2SystemAPI::logInternal(
	LogLevel logLevel, std::string tag, std::string msg) {
	SDL_LogMessage(
		SDL_LOG_CATEGORY_CUSTOM, getLogPriority(logLevel), "[%s] %s",
		tag.c_str(), msg.c_str());
}

SDL_LogPriority SDL2SystemAPI::getLogPriority(LogLevel logLevel) {
	switch (logLevel) {
	case LogLevel::DEBUG:
		return SDL_LOG_PRIORITY_DEBUG;
	case LogLevel::INFO:
		return SDL_LOG_PRIORITY_INFO;
	case LogLevel::WARN:
		return SDL_LOG_PRIORITY_WARN;
	case LogLevel::ERROR:
		return SDL_LOG_PRIORITY_ERROR;
	case LogLevel::FATAL:
		return SDL_LOG_PRIORITY_CRITICAL;
	}
}

bool SDL2SystemAPI::isRunning() {
	return running;
}
