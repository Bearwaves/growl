#include "ios_system.h"
#include "growl/core/assets/local_file.h"
#include "growl/core/error.h"
#include "growl/core/input/event.h"
#include "ios_error.h"
#include "ios_window.h"
#include <os/log.h>
#include <sys/utsname.h>

using Growl::Config;
using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::Error;
using Growl::File;
using Growl::HapticsDevice;
using Growl::InputEvent;
using Growl::InputEventType;
using Growl::InputTouchEvent;
using Growl::IOSSystemAPI;
using Growl::LocalFile;
using Growl::Result;
using Growl::SystemAPIInternal;
using Growl::Window;

std::unique_ptr<SystemAPIInternal>
Growl::createSystemAPI(API& api, void* user) {
	return std::make_unique<IOSSystemAPI>(api);
}

Error IOSSystemAPI::init(const Config& config) {
	for (GCController* controller in [GCController controllers]) {
		this->controller = controller;
		break; // Just get the first one, for now.
	}
	if (this->controller) {
		openGameController(this->controller);
		this->controller_haptics =
			std::make_unique<IOSHapticsDevice>(*this, this->controller);
	}
	this->device_haptics = std::make_unique<IOSHapticsDevice>(*this);

	NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
	game_controller_connect_observer =
		[center addObserverForName:GCControllerDidConnectNotification
							object:nil
							 queue:nil
						usingBlock:^(NSNotification* note) {
						  GCController* controller = note.object;
						  if (!this->controller) {
							  this->controller = controller;
							  openGameController(this->controller);
							  if (this->controller_haptics) {
								  this->controller_haptics->setGameController(
									  this->controller);
							  } else {
								  this->controller_haptics =
									  std::make_unique<IOSHapticsDevice>(
										  *this, this->controller);
							  }
						  }
						}];

	game_controller_disconnect_observer =
		[center addObserverForName:GCControllerDidDisconnectNotification
							object:nil
							 queue:nil
						usingBlock:^(NSNotification* note) {
						  GCController* controller = note.object;
						  if (controller == this->controller) {
							  closeGameController(this->controller);
							  this->controller = nullptr;
							  this->controller_haptics.reset();
						  }
						}];

	auto local_prefs_string = [[NSUserDefaults standardUserDefaults]
		stringForKey:@"preferences_local"];
	if (!local_prefs_string) {
		local_prefs_string = @"{}";
	}
	json j_local = json::parse([local_prefs_string UTF8String], nullptr, false);
	if (!j_local.is_object()) {
		j_local = {};
	}
	this->local_preferences =
		std::make_unique<IOSPreferences>(false, false, std::move(j_local));

	NSString* shared_prefs_string;
	bool ubiquitous = false;
	if ([[NSUbiquitousKeyValueStore defaultStore] synchronize]) {
		ubiquitous = true;
		shared_prefs_string = [[NSUbiquitousKeyValueStore defaultStore]
			stringForKey:@"preferences_shared"];
	} else {
		this->log(
			"IOSSystemAPI", "Unable to get initialise shared preferences; "
							"falling back to local storage.");
		shared_prefs_string = [[NSUserDefaults standardUserDefaults]
			stringForKey:@"preferences_shared"];
	}
	if (!shared_prefs_string) {
		shared_prefs_string = @"{}";
	}
	json j_shared =
		json::parse([shared_prefs_string UTF8String], nullptr, false);
	if (!j_shared.is_object()) {
		j_shared = {};
	}
	this->shared_preferences =
		std::make_unique<IOSPreferences>(ubiquitous, true, std::move(j_shared));
	this->has_shared_preferences = ubiquitous;

	this->log("IOSSystemAPI", "Initialised iOS system");
	return nullptr;
}

void IOSSystemAPI::tick() {}

void IOSSystemAPI::resume() {
	device_haptics->restart();
	if (controller_haptics) {
		controller_haptics->restart();
	}
	SystemAPIInternal::resume();
}

void IOSSystemAPI::dispose() {
	NSNotificationCenter* center = [NSNotificationCenter defaultCenter];
	if (game_controller_connect_observer) {
		[center removeObserver:game_controller_connect_observer
						  name:GCControllerDidConnectNotification
						object:nil];
	}
	if (game_controller_disconnect_observer) {
		[center removeObserver:game_controller_disconnect_observer
						  name:GCControllerDidDisconnectNotification
						object:nil];
	}
}

void IOSSystemAPI::onTouch(InputTouchEvent event) {
	if (inputProcessor) {
		InputEvent e{InputEventType::Touch, event};
		inputProcessor->onEvent(e);
	}
}

Result<std::unique_ptr<Window>>
IOSSystemAPI::createWindow(const Config& config) {
	UIWindow* w = [[[UIApplication sharedApplication] delegate] window];
	return std::unique_ptr<Window>(std::make_unique<IOSWindow>(w));
}

std::string IOSSystemAPI::getPlatformName() {
	return "ios";
}

std::string IOSSystemAPI::getPlatformOSVersion() {
	return [[UIDevice currentDevice].systemVersion UTF8String];
}

std::string IOSSystemAPI::getDeviceManufacturer() {
	return "Apple";
}

std::string IOSSystemAPI::getDeviceModel() {
	utsname info;
	uname(&info);
	return std::string(info.machine);
}

void IOSSystemAPI::setLogLevel(LogLevel log_level) {}

HapticsDevice* IOSSystemAPI::getHaptics() {
	if (this->controller_haptics) {
		return this->controller_haptics.get();
	}
	return this->device_haptics.get();
}

void IOSSystemAPI::logInternal(
	LogLevel log_level, std::string tag, std::string msg) {
	os_log_with_type(
		OS_LOG_DEFAULT, OS_LOG_TYPE_DEBUG, "[%s] %s", tag.c_str(), msg.c_str());
}

Result<std::unique_ptr<File>>
IOSSystemAPI::openFile(std::string path, size_t start, size_t end) {
	auto full_path =
		[[NSBundle mainBundle]
			pathForResource:[NSString stringWithUTF8String:path.c_str()]
					 ofType:@""]
			.UTF8String;
	std::ifstream file;
	file.open(full_path, std::ios::binary | std::ios::in);
	if (file.fail()) {
		return Error(std::make_unique<IOSError>("Failed to open file " + path));
	}
	if (end == 0) {
		auto ptr = file.tellg();
		file.seekg(0, std::ios::end);
		end = file.tellg();
		file.seekg(ptr);
	}
	return std::unique_ptr<File>(
		std::make_unique<LocalFile>(std::move(file), start, end));
}

void IOSSystemAPI::openGameController(GCController* controller) {
	this->log(
		"IOSSystemAPI", "Connected controller: {}",
		[controller.productCategory UTF8String]);
	if (controller.extendedGamepad) {
		controller.extendedGamepad.valueChangedHandler =
			^(GCExtendedGamepad* _Nonnull gamepad,
			  GCControllerElement* _Nonnull element) {
			  handleControllerInput(gamepad, element);
			};
		// iOS models dpad as axes, not buttons
		controller.extendedGamepad.dpad.down.valueChangedHandler =
			^(GCControllerButtonInput* _Nonnull button, float value,
			  BOOL pressed) {
			  dispatchControllerEvent(
				  ControllerButton::DpadDown,
				  controllerEventTypeForButtonPressed(pressed));
			};
		controller.extendedGamepad.dpad.up.valueChangedHandler =
			^(GCControllerButtonInput* _Nonnull button, float value,
			  BOOL pressed) {
			  dispatchControllerEvent(
				  ControllerButton::DpadUp,
				  controllerEventTypeForButtonPressed(pressed));
			};
		controller.extendedGamepad.dpad.left.valueChangedHandler =
			^(GCControllerButtonInput* _Nonnull button, float value,
			  BOOL pressed) {
			  dispatchControllerEvent(
				  ControllerButton::DpadLeft,
				  controllerEventTypeForButtonPressed(pressed));
			};
		controller.extendedGamepad.dpad.right.valueChangedHandler =
			^(GCControllerButtonInput* _Nonnull button, float value,
			  BOOL pressed) {
			  dispatchControllerEvent(
				  ControllerButton::DpadRight,
				  controllerEventTypeForButtonPressed(pressed));
			};
	}
}

void IOSSystemAPI::closeGameController(GCController* controller) {
	this->log(
		"IOSSystemAPI", "Disconnected controller: {}",
		[controller.productCategory UTF8String]);
}

void IOSSystemAPI::handleControllerInput(
	GCExtendedGamepad* gamepad, GCControllerElement* element) {
	if (!inputProcessor) {
		return;
	}
	if (element == gamepad.buttonA) {
		return dispatchControllerEvent(
			ControllerButton::A,
			controllerEventTypeForButtonPressed(gamepad.buttonA.pressed));
	}
	if (element == gamepad.buttonB) {
		return dispatchControllerEvent(
			ControllerButton::B,
			controllerEventTypeForButtonPressed(gamepad.buttonB.pressed));
	}
	if (element == gamepad.buttonX) {
		return dispatchControllerEvent(
			ControllerButton::X,
			controllerEventTypeForButtonPressed(gamepad.buttonX.pressed));
	}
	if (element == gamepad.buttonY) {
		return dispatchControllerEvent(
			ControllerButton::Y,
			controllerEventTypeForButtonPressed(gamepad.buttonY.pressed));
	}
	if (element == gamepad.leftShoulder) {
		return dispatchControllerEvent(
			ControllerButton::LB,
			controllerEventTypeForButtonPressed(gamepad.leftShoulder.pressed));
	}
	if (element == gamepad.rightShoulder) {
		return dispatchControllerEvent(
			ControllerButton::RB,
			controllerEventTypeForButtonPressed(gamepad.rightShoulder.pressed));
	}
	if (element == gamepad.leftTrigger) {
		return dispatchControllerEvent(
			ControllerButton::LT,
			controllerEventTypeForButtonPressed(gamepad.leftTrigger.pressed));
	}
	if (element == gamepad.rightTrigger) {
		return dispatchControllerEvent(
			ControllerButton::RT,
			controllerEventTypeForButtonPressed(gamepad.rightTrigger.pressed));
	}
	if (element == gamepad.leftThumbstickButton) {
		return dispatchControllerEvent(
			ControllerButton::LeftStick,
			controllerEventTypeForButtonPressed(
				gamepad.leftThumbstickButton.pressed));
	}
	if (element == gamepad.rightThumbstickButton) {
		return dispatchControllerEvent(
			ControllerButton::RightStick,
			controllerEventTypeForButtonPressed(
				gamepad.rightThumbstickButton.pressed));
	}
	if (element == gamepad.buttonMenu) {
		return dispatchControllerEvent(
			ControllerButton::Start,
			controllerEventTypeForButtonPressed(gamepad.buttonMenu.pressed));
	}
	if (element == gamepad.buttonOptions) {
		return dispatchControllerEvent(
			ControllerButton::Select,
			controllerEventTypeForButtonPressed(gamepad.buttonOptions.pressed));
	}
	if (element == gamepad.buttonHome) {
		return dispatchControllerEvent(
			ControllerButton::Home,
			controllerEventTypeForButtonPressed(gamepad.buttonHome.pressed));
	}
}

void IOSSystemAPI::dispatchControllerEvent(
	ControllerButton button, ControllerEventType event_type) {
	if (!inputProcessor) {
		return;
	}
	auto event = InputEvent{
		InputEventType::Controller,
		InputControllerEvent{.button = button, .type = event_type}};
	inputProcessor->onEvent(event);
}

ControllerEventType
IOSSystemAPI::controllerEventTypeForButtonPressed(bool pressed) {
	return pressed ? ControllerEventType::ButtonDown
				   : ControllerEventType::ButtonUp;
}
