#include "sdl2_system.h"
#include "SDL.h"
#include "SDL_rwops.h"
#include "growl/core/assets/file.h"
#include "growl/core/input/event.h"
#include "sdl2_file.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#include "imgui_impl_sdl.h"
#endif
#include "sdl2_error.h"
#include "sdl2_window.h"
#include <assert.h>
#include <memory>

using Growl::Error;
using Growl::File;
using Growl::InputEvent;
using Growl::Result;
using Growl::SDL2SystemAPI;
using Growl::Window;

Error SDL2SystemAPI::init() {
	if (SDL_WasInit(SDL_INIT_VIDEO) != 0) {
		return std::make_unique<SDL2Error>(SDL_GetError());
	}
	SDL_LogSetPriority(SDL_LOG_CATEGORY_CUSTOM, SDL_LOG_PRIORITY_INFO);
	if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) == -1) {
		return std::make_unique<SDL2Error>(SDL_GetError());
	}
	SDL_ShowCursor(SDL_DISABLE);

#ifdef GROWL_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	imgui_io = &ImGui::GetIO();
#endif

	this->log("SDL2SystemAPI", "Initialised SDL system");

	running = true;
	return nullptr;
}

Result<std::unique_ptr<Window>>
SDL2SystemAPI::createWindow(const WindowConfig& config) {
	int flags = SDL_WINDOW_INPUT_FOCUS;
	flags |= SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL |
			 SDL_WINDOW_ALLOW_HIGHDPI;

	SDL_Window* win = SDL_CreateWindow(
		config.getTitle().c_str(),
		config.isCentred() ? SDL_WINDOWPOS_CENTERED : 0,
		config.isCentred() ? SDL_WINDOWPOS_CENTERED : 0, config.getWidth(),
		config.getHeight(), flags);
	if (win == nullptr) {
		return Error(std::make_unique<SDL2Error>(SDL_GetError()));
	}

	return std::unique_ptr<Window>(std::make_unique<SDL2Window>(win));
}

void SDL2SystemAPI::tick() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
#ifdef GROWL_IMGUI
		if (api.imguiVisible()) {
			ImGui_ImplSDL2_ProcessEvent(&event);
		}
#endif
		switch (event.type) {
		case SDL_QUIT: {
			log("SDL2SystemAPI", "Got stop signal");
			running = false;
			break;
		}
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			handleMouseEvent(event);
			break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			handleKeyboardEvent(event);
			break;
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP:
			handleControllerEvent(event);
			break;
		case SDL_CONTROLLERDEVICEADDED:
			openGameController(event.cdevice.which);
			break;
		}
	}
}

void SDL2SystemAPI::dispose() {
	controller = nullptr;
	SDL_Quit();
#ifdef GROWL_IMGUI
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
#endif
}

void SDL2SystemAPI::setLogLevel(LogLevel log_level) {
	SDL_LogSetPriority(SDL_LOG_CATEGORY_CUSTOM, getLogPriority(log_level));
}

Result<std::unique_ptr<File>>
SDL2SystemAPI::openFile(std::string path, size_t start, size_t end) {
	auto fp = SDL_RWFromFile(path.c_str(), "rb");
	if (!fp) {
		return Error(std::make_unique<SDL2Error>(SDL_GetError()));
	}

	if (end == 0) {
		end = SDL_RWsize(fp);
	}

	return std::unique_ptr<File>(std::make_unique<SDL2File>(fp, start, end));
}

void SDL2SystemAPI::logInternal(
	LogLevel log_level, std::string tag, std::string msg) {
	SDL_LogMessage(
		SDL_LOG_CATEGORY_CUSTOM, getLogPriority(log_level), "[%s] %s",
		tag.c_str(), msg.c_str());
}

SDL_LogPriority SDL2SystemAPI::getLogPriority(LogLevel log_level) {
	switch (log_level) {
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
	return SDL_LOG_PRIORITY_VERBOSE;
}

void SDL2SystemAPI::handleMouseEvent(SDL_Event& event) {
	if (inputProcessor
#ifdef GROWL_IMGUI
		&& !(api.imguiVisible() && imgui_io->WantCaptureMouse)
#endif
	) {
		int display_w, display_h, window_w, window_h;
		SDL_Window* window = SDL_GetWindowFromID(event.motion.windowID);
		SDL_GetWindowSize(window, &window_w, &window_h);
		SDL_GL_GetDrawableSize(window, &display_w, &display_h);
		int x = event.motion.x * (display_w / (float)window_w);
		int y = event.motion.y * (display_h / (float)window_h);
		InputEvent e(InputEventType::MOUSE, InputMouseEvent{x, y});
		inputProcessor->onEvent(e);
	}
}
