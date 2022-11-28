#pragma once

#include "growl/core/api/api.h"
#include "growl/core/api/api_internal.h"
#include "growl/core/input/controller.h"
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
	bool didResize(int* width, int* height) override;

	Result<std::unique_ptr<File>>
	openFile(std::string path, size_t start, size_t end) override;

private:
	void
	logInternal(LogLevel log_level, std::string tag, std::string msg) override;
	int logPriorityForLevel(LogLevel log_level);
	void onControllerEvent(InputControllerEvent event);
	void onResizeEvent(int width, int height);
	static void handleAppCmd(android_app* app, int32_t cmd);
	static int32_t handleInput(android_app* app, AInputEvent* cmd);
	static PointerEventType getPointerEventType(AInputEvent* event);
	static ControllerButton getControllerButton(AInputEvent* event);
	static ControllerEventType getControllerEventType(AInputEvent* event);

	API& api;
	android_app* android_state;
	int resize_width = 0;
	int resize_height = 0;
};

} // namespace Growl
