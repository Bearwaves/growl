#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/input/event.h"
#include <android/input.h>
#include <android_native_app_glue.h>

namespace Growl {

class AndroidSystemAPI : public SystemAPIInternal {
public:
	explicit AndroidSystemAPI(API& api, android_app* android_state)
		: api{api}
		, android_state{android_state} {}
	Error init() override;
	void tick() override;
	void dispose() override;
	void onTouch(InputTouchEvent) override;
	bool isRunning() override {
		return android_state->destroyRequested == 0;
	}
	virtual Result<std::unique_ptr<Window>>
	createWindow(const WindowConfig& config) override;
	void setLogLevel(LogLevel log_level) override;

	Result<std::unique_ptr<File>>
	openFile(std::string path, size_t start, size_t end) override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;
	int logPriorityForLevel(LogLevel log_level);
	static void handleAppCmd(android_app* app, int32_t cmd);
	static int32_t handleInput(android_app* app, AInputEvent* cmd);
	static TouchEventType getTouchEventType(AInputEvent* event);

	API& api;
	android_app* android_state;
};

} // namespace Growl
