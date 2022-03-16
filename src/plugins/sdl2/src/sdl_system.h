#pragma once

#include <SDL.h>
#include <growl/core/api/api.h>
#include <memory>

namespace Growl {

class SDL2SystemAPI : public SystemAPIInternal {
public:
	Error init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override {
		return running;
	}
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel logLevel) override;

private:
	void
	logInternal(LogLevel logLevel, std::string tag, std::string msg) override;
	SDL_LogPriority getLogPriority(LogLevel logLevel);

	void handleMouseEvent(SDL_Event& event);
	void handleKeyboardEvent(SDL_Event& event);

	KeyEventType getKeyEventType(SDL_KeyboardEvent& event);
	Key getKey(SDL_KeyboardEvent& event);

	bool running;
};

} // namespace Growl
