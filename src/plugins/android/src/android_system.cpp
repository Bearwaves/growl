#include "android_system.h"
#include "android_file.h"
#include "android_window.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include "growl/core/input/event.h"
#include "growl/core/log.h"
#include <android/input.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <memory>

using Growl::AndroidFile;
using Growl::AndroidSystemAPI;
using Growl::Error;
using Growl::File;
using Growl::LogLevel;
using Growl::Result;
using Growl::TouchEventType;
using Growl::Window;

Error AndroidSystemAPI::init() {
	android_state->onAppCmd = handleAppCmd;
	android_state->onInputEvent = handleInput;
	android_state->userData = &api;
	this->log("AndroidSystemAPI", "Initialised Android system");
	return nullptr;
}

void AndroidSystemAPI::tick() {
	int ident;
	int events;
	struct android_poll_source* source;
	while ((ident = ALooper_pollAll(0, nullptr, &events, (void**)&source)) >=
		   0) {
		if (source != nullptr) {
			source->process(android_state, source);
		}
	}
}

void AndroidSystemAPI::handleAppCmd(android_app* app, int32_t cmd) {
	API* api = (API*)app->userData;
	switch (cmd) {
	case APP_CMD_INIT_WINDOW:
		api->system().log("AndroidSystemAPI", "Window is ready for creation");
		break;
	}
}

int32_t AndroidSystemAPI::handleInput(android_app* app, AInputEvent* event) {
	API* api = (API*)app->userData;
	switch (AInputEvent_getType(event)) {
	case AINPUT_EVENT_TYPE_MOTION:
		switch (AInputEvent_getSource(event)) {
		case AINPUT_SOURCE_TOUCHSCREEN:
			static_cast<AndroidSystemAPI&>(api->system())
				.onTouch(InputTouchEvent{
					getTouchEventType(event),
					static_cast<int>(AMotionEvent_getX(event, 0)),
					static_cast<int>(AMotionEvent_getY(event, 0)),
				});
			return 1;
		}
		return 0;
	}
	return 0;
}

TouchEventType AndroidSystemAPI::getTouchEventType(AInputEvent* event) {
	switch (AMotionEvent_getAction(event)) {
	case AMOTION_EVENT_ACTION_DOWN:
		return TouchEventType::DOWN;
	case AMOTION_EVENT_ACTION_UP:
		return TouchEventType::UP;
	case AMOTION_EVENT_ACTION_MOVE:
		return TouchEventType::MOVE;
	}
	return TouchEventType::UNKNOWN;
}

void AndroidSystemAPI::dispose() {}

void AndroidSystemAPI::onTouch(InputTouchEvent event) {
	if (!inputProcessor) {
		return;
	}
	if (event.type == TouchEventType::UNKNOWN) {
		return;
	}
	inputProcessor->onTouchEvent(event);
}

Result<std::unique_ptr<Window>>
AndroidSystemAPI::createWindow(const WindowConfig& config) {
	return std::unique_ptr<Window>(
		std::make_unique<AndroidWindow>(android_state->window));
}

void AndroidSystemAPI::setLogLevel(LogLevel log_level) {}

void AndroidSystemAPI::logInternal(
	LogLevel log_level, std::string tag, std::string msg) {
	__android_log_print(
		logPriorityForLevel(log_level), tag.c_str(), "%s", msg.c_str());
}

Result<std::unique_ptr<File>>
AndroidSystemAPI::openFile(std::string path, size_t start, size_t end) {
	AAsset* asset = AAssetManager_open(
		android_state->activity->assetManager, path.c_str(),
		AASSET_MODE_RANDOM);
	if (!asset) {
		return Error(
			std::make_unique<AssetsError>("Failed to load asset " + path));
	}
	if (end == 0) {
		end = AAsset_getLength(asset);
	}
	return std::unique_ptr<File>(
		std::make_unique<AndroidFile>(asset, start, end));
}

int AndroidSystemAPI::logPriorityForLevel(LogLevel level) {
	switch (level) {
	case LogLevel::DEBUG:
		return ANDROID_LOG_DEBUG;
	case LogLevel::INFO:
		return ANDROID_LOG_INFO;
	case LogLevel::WARN:
		return ANDROID_LOG_WARN;
	case LogLevel::ERROR:
		return ANDROID_LOG_ERROR;
	case LogLevel::FATAL:
		return ANDROID_LOG_FATAL;
	}
}
