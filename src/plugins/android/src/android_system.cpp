#include "android_system.h"
#include "android_file.h"
#include "android_window.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "growl/core/log.h"
#include <android/input.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <memory>

using Growl::AndroidFile;
using Growl::AndroidSystemAPI;
using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::Error;
using Growl::File;
using Growl::InputControllerEvent;
using Growl::LogLevel;
using Growl::PointerEventType;
using Growl::Result;
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
	while ((ident = ALooper_pollOnce(0, nullptr, &events, (void**)&source)) >=
		   0) {
		if (source != nullptr) {
			source->process(android_state, source);
		}
	}
}

bool AndroidSystemAPI::didResize(int* width, int* height) {
	*width = resize_width;
	*height = resize_height;
	resize_width = 0;
	resize_height = 0;
	return *width || *height;
}

void AndroidSystemAPI::handleAppCmd(android_app* app, int32_t cmd) {
	API* api = (API*)app->userData;
	switch (cmd) {
	case APP_CMD_INIT_WINDOW:
		api->system().log("AndroidSystemAPI", "Window is ready for creation");
		break;
	case APP_CMD_WINDOW_RESIZED:
	case APP_CMD_CONFIG_CHANGED:
		static_cast<AndroidSystemAPI&>(api->system())
			.onResizeEvent(
				ANativeWindow_getWidth(app->window),
				ANativeWindow_getHeight(app->window));
		break;
	}
}

void AndroidSystemAPI::onResizeEvent(int width, int height) {
	resize_width = width;
	resize_height = height;
}

int32_t AndroidSystemAPI::handleInput(android_app* app, AInputEvent* event) {
	API* api = (API*)app->userData;
	switch (AInputEvent_getType(event)) {
	case AINPUT_EVENT_TYPE_MOTION:
		switch (AInputEvent_getSource(event)) {
		case AINPUT_SOURCE_TOUCHSCREEN:
			static_cast<AndroidSystemAPI&>(api->system())
				.onTouch(InputTouchEvent{
					getPointerEventType(event),
					static_cast<int>(AMotionEvent_getX(event, 0)),
					static_cast<int>(AMotionEvent_getY(event, 0)),
				});
			return 1;
		}
		break;
	case AINPUT_EVENT_TYPE_KEY:
		auto button = getControllerButton(event);
		if (button == ControllerButton::Unknown) {
			// Avoid capturing back button etc. for now.
			return 0;
		}
		static_cast<AndroidSystemAPI&>(api->system())
			.onControllerEvent(InputControllerEvent{
				getControllerEventType(event),
				button,
			});
		return 1;
	}
	return 0;
}

PointerEventType AndroidSystemAPI::getPointerEventType(AInputEvent* event) {
	switch (AMotionEvent_getAction(event)) {
	case AMOTION_EVENT_ACTION_DOWN:
		return PointerEventType::Down;
	case AMOTION_EVENT_ACTION_UP:
		return PointerEventType::Up;
	case AMOTION_EVENT_ACTION_MOVE:
		return PointerEventType::Move;
	}
	return PointerEventType::Unknown;
}

ControllerButton AndroidSystemAPI::getControllerButton(AInputEvent* event) {
	switch (AKeyEvent_getKeyCode(event)) {
	case AKEYCODE_BUTTON_A:
		return ControllerButton::A;
	case AKEYCODE_BUTTON_B:
		return ControllerButton::B;
	case AKEYCODE_BUTTON_X:
		return ControllerButton::X;
	case AKEYCODE_BUTTON_Y:
		return ControllerButton::Y;
	case AKEYCODE_BUTTON_L1:
		return ControllerButton::LB;
	case AKEYCODE_BUTTON_R1:
		return ControllerButton::RB;
	case AKEYCODE_BUTTON_L2:
		return ControllerButton::LT;
	case AKEYCODE_BUTTON_R2:
		return ControllerButton::RT;
	case AKEYCODE_DPAD_UP:
		return ControllerButton::DpadUp;
	case AKEYCODE_DPAD_DOWN:
		return ControllerButton::DpadDown;
	case AKEYCODE_DPAD_LEFT:
		return ControllerButton::DpadLeft;
	case AKEYCODE_DPAD_RIGHT:
		return ControllerButton::DpadRight;
	case AKEYCODE_BUTTON_THUMBL:
		return ControllerButton::LeftStick;
	case AKEYCODE_BUTTON_THUMBR:
		return ControllerButton::RightStick;
	case AKEYCODE_BUTTON_START:
		return ControllerButton::Start;
	case AKEYCODE_BUTTON_SELECT:
		return ControllerButton::Select;
	case AKEYCODE_BUTTON_MODE:
		return ControllerButton::Home;
	}
	return ControllerButton::Unknown;
}

ControllerEventType
AndroidSystemAPI::getControllerEventType(AInputEvent* event) {
	switch (AKeyEvent_getAction(event)) {
	case AKEY_EVENT_ACTION_DOWN:
		return ControllerEventType::ButtonDown;
	case AKEY_EVENT_ACTION_UP:
		return ControllerEventType::ButtonUp;
	}
	return ControllerEventType::Unknown;
}

void AndroidSystemAPI::dispose() {}

void AndroidSystemAPI::onTouch(InputTouchEvent event) {
	if (!inputProcessor || event.type == PointerEventType::Unknown) {
		return;
	}
	InputEvent e{InputEventType::Touch, event};
	inputProcessor->onEvent(e);
}

void AndroidSystemAPI::onControllerEvent(InputControllerEvent event) {
	if (!inputProcessor || event.type == ControllerEventType::Unknown ||
		event.button == ControllerButton::Unknown) {
		return;
	}
	InputEvent e{InputEventType::Controller, event};
	inputProcessor->onEvent(e);
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
	case LogLevel::Debug:
		return ANDROID_LOG_DEBUG;
	case LogLevel::Info:
		return ANDROID_LOG_INFO;
	case LogLevel::Warn:
		return ANDROID_LOG_WARN;
	case LogLevel::Error:
		return ANDROID_LOG_ERROR;
	case LogLevel::Fatal:
		return ANDROID_LOG_FATAL;
	}
}
