#pragma once

#include "growl/core/api/api.h"
#include "growl/core/assets/file.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif
#include <SDL.h>
#include <memory>

namespace Growl {

class SDL2Controller {
public:
	SDL2Controller(SystemAPI* system, SDL_GameController* controller)
		: system{system}
		, controller{controller} {}
	~SDL2Controller() {
		system->log(
			"SDL2Controller", "Closed controller: {}",
			SDL_GameControllerName(controller));
		SDL_GameControllerClose(controller);
	}

private:
	SystemAPI* system;
	SDL_GameController* controller;
};

class SDL2SystemAPI : public SystemAPIInternal {
public:
	explicit SDL2SystemAPI(API& api)
		: api{api} {}
	Error init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override {
		return running;
	}
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel log_level) override;

	Result<std::unique_ptr<File>>
	openFile(std::string path, size_t start, size_t end) override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;
	SDL_LogPriority getLogPriority(LogLevel log_level);

	void handleMouseEvent(SDL_Event& event);
	void handleKeyboardEvent(SDL_Event& event);
	void handleControllerEvent(SDL_Event& event);

	KeyEventType getKeyEventType(SDL_KeyboardEvent& event);
	Key getKey(SDL_KeyboardEvent& event);

	ControllerEventType getControllerEventType(SDL_Event& event);
	ControllerButton getButton(SDL_Event& event);
	void openGameController(int id);

	API& api;
	bool running;
	std::unique_ptr<SDL2Controller> controller;

#ifdef GROWL_IMGUI
	ImGuiIO* imgui_io;
#endif
};

} // namespace Growl
