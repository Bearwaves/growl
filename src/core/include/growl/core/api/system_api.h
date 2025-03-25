#pragma once

#include "../log.h"
#include "fmt/color.h"
#include "fmt/core.h"
#include "growl/core/error.h"
#include "growl/core/input/processor.h"
#include <memory>
#include <string>
#include <string_view>

namespace Growl {

struct Config;
class File;
class Window;

class SystemAPI {
	friend class SystemAPIInternal;

public:
	virtual ~SystemAPI() {}
	virtual bool isRunning() = 0;
	virtual bool isPaused() {
		return paused;
	}
	virtual void stop() {}
	virtual void tick() = 0;
	virtual Result<std::unique_ptr<Window>>
	createWindow(const Config& config) = 0;
	virtual void setInputProcessor(InputProcessor* processor) {
		inputProcessor = processor;
	}
	virtual bool didResize(int* width, int* height) {
		return false;
	}

	template <class... Args>
	void
	log(LogLevel log_level, std::string tag, std::string_view msg,
		Args&&... args) {
		logInternal(
			log_level, tag, fmt::format(msg, std::forward<Args>(args)...));
	}

	template <class... Args>
	void log(std::string tag, std::string_view msg, Args&&... args) {
		log(LogLevel::Info, tag, msg, args...);
	}

	virtual void setLogLevel(LogLevel log_level) = 0;

	virtual bool isDarkMode() {
		return dark_mode;
	}

	virtual Result<std::unique_ptr<File>>
	openFile(std::string path, size_t start = 0, size_t end = 0) = 0;

protected:
	virtual void
	logInternal(LogLevel log_level, std::string tag, std::string formatted) = 0;
	InputProcessor* inputProcessor = nullptr;

private:
	bool dark_mode = false;
	bool paused = false;
	bool mouse_emits_touch_events = false;
};

} // namespace Growl
