#pragma once

#include "SDL_events.h"
#include "SDL_gamecontroller.h"
#include "SDL_log.h"
#include "growl/core/api/api_internal.h"
#include <memory>

struct ImGuiIO;

namespace Growl {

class API;
class File;
class Window;
class WindowConfig;
enum class ControllerButton;
enum class ControllerEventType;
enum class Key;
enum class KeyEventType;
enum class LogLevel;

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
	void stop() override;
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel log_level) override;
	bool didResize(int* width, int* height) override;

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
	int resize_width = 0;
	int resize_height = 0;

#ifdef GROWL_IMGUI
	ImGuiIO* imgui_io;
#endif
};

} // namespace Growl
