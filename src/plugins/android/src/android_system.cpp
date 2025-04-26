#include "android_system.h"
#include "android_error.h"
#include "android_file.h"
#include "android_window.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "growl/core/log.h"
#include <android/configuration.h>
#include <android/input.h>
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>
#include <paddleboat/paddleboat.h>

using Growl::AndroidError;
using Growl::AndroidFile;
using Growl::AndroidSystemAPI;
using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::Error;
using Growl::File;
using Growl::HapticsDevice;
using Growl::InputControllerEvent;
using Growl::LogLevel;
using Growl::PointerEventType;
using Growl::Result;
using Growl::Window;

constexpr int DPAD_UP_MASK = (1 << static_cast<int>(ControllerButton::DpadUp));
constexpr int DPAD_DOWN_MASK =
	(1 << static_cast<int>(ControllerButton::DpadDown));
constexpr int DPAD_LEFT_MASK =
	(1 << static_cast<int>(ControllerButton::DpadLeft));
constexpr int DPAD_RIGHT_MASK =
	(1 << static_cast<int>(ControllerButton::DpadRight));

Error AndroidSystemAPI::init(const Config& config) {
	android_state->onAppCmd = handleAppCmd;
	android_state->userData = &api;
	// Leave key event filter as default to allow volume keys to propagate
	// back up to system. Clear motion filter so controller events get through.
	android_app_set_motion_event_filter(android_state, nullptr);

	GameActivityPointerAxes_enableAxis(AMOTION_EVENT_AXIS_HAT_X);
	GameActivityPointerAxes_enableAxis(AMOTION_EVENT_AXIS_HAT_Y);

	if (auto err = Paddleboat_init(
			getJNIEnv(), android_state->activity->javaGameActivity);
		err != PADDLEBOAT_NO_ERROR) {
		return std::make_unique<AndroidError>(
			"Failed to init GameController library. Error code: " +
			std::to_string(err));
	}
	Paddleboat_setControllerStatusCallback(controllerStatusCallback, this);

	this->device_haptics = std::make_unique<AndroidHaptics>(android_state);

	json j_local = json::parse(
		AndroidPreferences::getPreferencesJSON(android_state, false), nullptr,
		false);
	if (!j_local.is_object()) {
		j_local = {};
	}
	this->preferences_local = std::make_unique<AndroidPreferences>(
		android_state, false, std::move(j_local));

	json j_shared = json::parse(
		AndroidPreferences::getPreferencesJSON(android_state, true), nullptr,
		false);
	if (!j_shared.is_object()) {
		j_shared = {};
	}
	this->preferences_shared = std::make_unique<AndroidPreferences>(
		android_state, true, std::move(j_shared));

	this->log("AndroidSystemAPI", "Initialised Android system");
	return nullptr;
}

void AndroidSystemAPI::tick() {
	Paddleboat_update(getJNIEnv());
	int events;
	struct android_poll_source* source;
	while ((ALooper_pollOnce(0, nullptr, &events, (void**)&source)) >= 0) {
		if (source != nullptr) {
			source->process(android_state, source);
		}
	}
	handleInput(android_state);
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
	case APP_CMD_INIT_WINDOW: {
		api->system().log("AndroidSystemAPI", "Window is ready for creation");
		if (!app->window) {
			api->system().log("AndroidSystemAPI", "No native window");
			break;
		}
		auto& graphics_internal =
			static_cast<GraphicsAPIInternal&>(api->graphics());
		if (!graphics_internal.getWindow()) {
			Config config;
			if (auto err = graphics_internal.setWindow(config)) {
				api->system().log(
					LogLevel::Fatal, "android_main",
					"Failed to create window: {}", err.get()->message());
				break;
			}
		}
		if (auto err =
				static_cast<AndroidWindow*>(graphics_internal.getWindow())
					->initSurface()) {
			api->system().log(
				LogLevel::Error, "AndroidSystemAPI",
				"Failed to init surface " + err->message());
			break;
		}
		break;
	}
	case APP_CMD_TERM_WINDOW:
		api->system().log("AndroidSystemAPI", "Window needs termination");
		if (auto err = static_cast<AndroidWindow*>(
						   static_cast<GraphicsAPIInternal&>(api->graphics())
							   .getWindow())
						   ->deinitSurface()) {
			api->system().log(
				LogLevel::Error, "AndroidSystemAPI",
				"Failed to deinit surface " + err->message());
		}
		break;
	case APP_CMD_WINDOW_RESIZED:
	case APP_CMD_CONFIG_CHANGED: {
		auto& system_internal = static_cast<AndroidSystemAPI&>(api->system());
		if (app->window) {
			system_internal.onResizeEvent(
				ANativeWindow_getWidth(app->window),
				ANativeWindow_getHeight(app->window));
		}
		system_internal.setDarkMode(getDarkMode(app));
		break;
	}
	case APP_CMD_PAUSE:
		api->audio().setMuted(true);
		static_cast<SystemAPIInternal&>(api->system()).pause();
		break;
	case APP_CMD_RESUME:
		api->audio().setMuted(false);
		static_cast<SystemAPIInternal&>(api->system()).resume();
		break;
	case APP_CMD_START: {
		JNIEnv* env;
		app->activity->vm->AttachCurrentThread(&env, nullptr);
		Paddleboat_onStart(env);
		break;
	}
	case APP_CMD_STOP: {
		JNIEnv* env;
		app->activity->vm->AttachCurrentThread(&env, nullptr);
		Paddleboat_onStop(env);
		break;
	}
	}
}

void AndroidSystemAPI::onResizeEvent(int width, int height) {
	resize_width = width;
	resize_height = height;
}

void AndroidSystemAPI::handleInput(android_app* app) {
	API* api = (API*)app->userData;
	auto& this_api = static_cast<AndroidSystemAPI&>(api->system());
	auto ib = android_app_swap_input_buffers(app);

	if (ib && ib->motionEventsCount) {
		for (int i = 0; i < ib->motionEventsCount; i++) {
			auto* event = &ib->motionEvents[i];
			switch (event->source & AINPUT_SOURCE_CLASS_MASK) {

			case AINPUT_SOURCE_CLASS_POINTER:
				this_api.onTouch(
					InputTouchEvent{
						getPointerEventType(event->action),
						static_cast<int>(GameActivityPointerAxes_getAxisValue(
							&event->pointers[0], AMOTION_EVENT_AXIS_X)),
						static_cast<int>(GameActivityPointerAxes_getAxisValue(
							&event->pointers[0], AMOTION_EVENT_AXIS_Y)),
					});
				break;

			case AINPUT_SOURCE_CLASS_JOYSTICK: {
				int dpad_state = 0;
				float x = GameActivityPointerAxes_getAxisValue(
					&event->pointers[0], AMOTION_EVENT_AXIS_HAT_X);
				float y = GameActivityPointerAxes_getAxisValue(
					&event->pointers[0], AMOTION_EVENT_AXIS_HAT_Y);
				if (x < 0) {
					dpad_state |= DPAD_LEFT_MASK;
				} else if (x > 0) {
					dpad_state |= DPAD_RIGHT_MASK;
				}
				if (y < 0) {
					dpad_state |= DPAD_UP_MASK;
				} else if (y > 0) {
					dpad_state |= DPAD_DOWN_MASK;
				}

				int dpad_delta = (dpad_state ^ this_api.dpad_state);
				if (dpad_delta) {
					this_api.dpad_state = dpad_state;
					if (dpad_delta & DPAD_UP_MASK) {
						bool down = ((dpad_state & DPAD_UP_MASK) != 0);
						this_api.onControllerEvent(
							InputControllerEvent{
								down ? ControllerEventType::ButtonDown
									 : ControllerEventType::ButtonUp,
								ControllerButton::DpadUp,
							});
					}
					if (dpad_delta & DPAD_DOWN_MASK) {
						bool down = ((dpad_state & DPAD_DOWN_MASK) != 0);
						this_api.onControllerEvent(
							InputControllerEvent{
								down ? ControllerEventType::ButtonDown
									 : ControllerEventType::ButtonUp,
								ControllerButton::DpadDown,
							});
					}
					if (dpad_delta & DPAD_LEFT_MASK) {
						bool down = ((dpad_state & DPAD_LEFT_MASK) != 0);
						this_api.onControllerEvent(
							InputControllerEvent{
								down ? ControllerEventType::ButtonDown
									 : ControllerEventType::ButtonUp,
								ControllerButton::DpadLeft,
							});
					}
					if (dpad_delta & DPAD_RIGHT_MASK) {
						bool down = ((dpad_state & DPAD_RIGHT_MASK) != 0);
						this_api.onControllerEvent(
							InputControllerEvent{
								down ? ControllerEventType::ButtonDown
									 : ControllerEventType::ButtonUp,
								ControllerButton::DpadRight,
							});
					}
				}
				break;
			}
			}
		}
		android_app_clear_motion_events(ib);
	}

	if (ib && ib->keyEventsCount) {
		for (int i = 0; i < ib->keyEventsCount; i++) {
			auto* event = &ib->keyEvents[i];
			auto button = getControllerButton(event->keyCode);
			if (button == ControllerButton::Unknown) {
				// Avoid capturing back button etc. for now.
				continue;
			}
			this_api.onControllerEvent(
				InputControllerEvent{
					getControllerEventType(event->action),
					button,
				});
		}
		android_app_clear_key_events(ib);
	}
}

PointerEventType AndroidSystemAPI::getPointerEventType(int32_t action) {
	switch (action & AMOTION_EVENT_ACTION_MASK) {
	case AMOTION_EVENT_ACTION_DOWN:
		return PointerEventType::Down;
	case AMOTION_EVENT_ACTION_UP:
		return PointerEventType::Up;
	case AMOTION_EVENT_ACTION_MOVE:
		return PointerEventType::Move;
	}
	return PointerEventType::Unknown;
}

ControllerButton AndroidSystemAPI::getControllerButton(int32_t key_code) {
	switch (key_code) {
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

ControllerEventType AndroidSystemAPI::getControllerEventType(int32_t action) {
	switch (action) {
	case AKEY_EVENT_ACTION_DOWN:
		return ControllerEventType::ButtonDown;
	case AKEY_EVENT_ACTION_UP:
		return ControllerEventType::ButtonUp;
	}
	return ControllerEventType::Unknown;
}

void AndroidSystemAPI::dispose() {
	Paddleboat_destroy(getJNIEnv());
}

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
AndroidSystemAPI::createWindow(const Config& config) {
	return std::unique_ptr<Window>(
		std::make_unique<AndroidWindow>(android_state->window, android_state));
}

void AndroidSystemAPI::setLogLevel(LogLevel log_level) {}

HapticsDevice* AndroidSystemAPI::getHaptics() {
	if (controller_haptics) {
		return controller_haptics.get();
	}
	return device_haptics.get();
}

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

bool AndroidSystemAPI::getDarkMode(android_app* app) {
	JNIEnv* env{};
	app->activity->vm->AttachCurrentThread(&env, nullptr);
	jclass activity_class =
		env->GetObjectClass(app->activity->javaGameActivity);
	jmethodID method = env->GetMethodID(activity_class, "getDarkMode", "()Z");
	jboolean dark_mode =
		env->CallBooleanMethod(app->activity->javaGameActivity, method);
	return dark_mode == JNI_TRUE;
}

void AndroidSystemAPI::controllerStatusCallback(
	const int32_t controller_index,
	const Paddleboat_ControllerStatus controller_status, void* userdata) {
	AndroidSystemAPI* api = static_cast<AndroidSystemAPI*>(userdata);
	if (controller_index > 0) {
		// We don't support multiple controllers yet.
		return;
	}
	switch (controller_status) {
	case PADDLEBOAT_CONTROLLER_JUST_CONNECTED: {
		Paddleboat_Controller_Info info;
		auto err = Paddleboat_getControllerInfo(controller_index, &info);
		if (err) {
			api->log(
				LogLevel::Error, "AndroidSystemAPI",
				"Error getting controller info: {}", static_cast<int>(err));
			break;
		}
		api->controller_haptics =
			std::make_unique<AndroidHaptics>(api->android_state, &info);
		api->log(
			"AndroidSystemAPI", "Connected new controller with device ID {}",
			info.deviceId);
		break;
	}
	case PADDLEBOAT_CONTROLLER_JUST_DISCONNECTED:
		api->controller_haptics.reset();
		api->log(
			"AndroidSystemAPI", "Controller {} disconnected", controller_index);
		break;
	default:
		break;
	}
}

JNIEnv* AndroidSystemAPI::getJNIEnv() {
	if (!app_jni_env) {
		android_state->activity->vm->AttachCurrentThread(&app_jni_env, nullptr);
	}
	return app_jni_env;
}
