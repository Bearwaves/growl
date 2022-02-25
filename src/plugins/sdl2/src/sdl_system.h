#pragma once

#include <SDL.h>
#include <growl/core/api/api.h>
#include <memory>

namespace Growl {

class SDL2SystemAPI : public SystemAPIInternal {
public:
	void init() override;
	void tick() override;
	void dispose() override;
	bool isRunning() override;
	virtual std::unique_ptr<Window>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel logLevel) override;

private:
	void
	logInternal(LogLevel logLevel, std::string tag, std::string msg) override;
	SDL_LogPriority getLogPriority(LogLevel logLevel);
	bool running;
};

} // namespace Growl
