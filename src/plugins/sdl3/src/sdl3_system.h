#pragma once

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_log.h"
#include "growl/core/api/api_internal.h"
#include <memory>

struct ImGuiIO;

namespace Growl {

class API;
class File;
class Window;
struct Config;
enum class ControllerButton;
enum class ControllerEventType;
enum class Key;
enum class KeyEventType;
enum class LogLevel;

class SDL3Controller {
public:
	SDL3Controller(SystemAPI* system, SDL_Gamepad* controller)
		: system{system}
		, controller{controller} {}
	~SDL3Controller() {
		system->log(
			"SDL3Controller", "Closed controller: {}",
			SDL_GetGamepadName(controller));
		SDL_CloseGamepad(controller);
	}

private:
	SystemAPI* system;
	SDL_Gamepad* controller;
};

class SDL3SystemAPI : public SystemAPIInternal {
public:
	explicit SDL3SystemAPI(API& api)
		: api{api} {}
	Error init(const Config& config) override;
	void tick() override;
	void dispose() override;
	bool isRunning() override {
		return running;
	}
	void stop() override;
	virtual Result<std::unique_ptr<Window>>
	createWindow(const Config& config) override;
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
	SDL_Scancode getScancode(Key key);

	ControllerEventType getControllerEventType(SDL_Event& event);
	ControllerButton getButton(SDL_Event& event);
	void openGameController(int id);

	API& api;
	bool running;
	std::unique_ptr<SDL3Controller> controller;
	int resize_width = 0;
	int resize_height = 0;
	SDL_Scancode debug_mode_key;

#ifdef GROWL_IMGUI
	ImGuiIO* imgui_io;
	uint32_t imgui_resize_window = 0;
#endif
};

} // namespace Growl
