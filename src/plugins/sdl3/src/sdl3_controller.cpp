#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gamepad.h"
#include "SDL3/SDL_joystick.h"
#include "growl/core/haptics.h"
#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "sdl3_haptics.h"
#include "sdl3_system.h"

using Growl::ControllerAxis;
using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::HapticsDevice;
using Growl::SDL3Controller;
using Growl::SDL3SystemAPI;

constexpr int JOYSTICK_DEADZONE = 8000;

void SDL3SystemAPI::openGameController(int id) {
	auto sdl_controller = SDL_OpenGamepad(id);
	log("SDL3SystemAPI", "Got controller: [{}] {}", id,
		SDL_GetGamepadName(sdl_controller));
	controllers[id] =
		std::make_unique<SDL3Controller>(this, sdl_controller, id);
}

bool SDL3SystemAPI::closeGameController(int id) {
	return controllers.erase(id);
}

void SDL3SystemAPI::handleControllerEvent(SDL_Event& event) {
	if (!inputProcessor) {
		return;
	}
	InputControllerEvent controller_event;
	controller_event.controller = event.gdevice.which;
	controller_event.type = getControllerEventType(event);
	if (controller_event.type == ControllerEventType::AxisMoved) {
		controller_event.axis = getAxis(event);
		auto value = event.gaxis.value;
		controller_event.value =
			value < -JOYSTICK_DEADZONE
				? -static_cast<float>(value) / SDL_JOYSTICK_AXIS_MIN
			: value > JOYSTICK_DEADZONE
				? static_cast<float>(value) / SDL_JOYSTICK_AXIS_MAX
				: 0.f;
	} else {
		controller_event.button = getButton(event);
	}
	InputEvent e{InputEventType::Controller, controller_event};
	inputProcessor->onEvent(e);
}

SDL3Controller::SDL3Controller(
	SystemAPI* system, SDL_Gamepad* controller, int id)
	: system{system}
	, controller{controller}
	, id{id}
	, haptics{std::make_unique<SDL3HapticsDevice>(system, controller)} {}

SDL3Controller::~SDL3Controller() {
	system->log(
		"SDL3Controller", "Closed controller: {}",
		SDL_GetGamepadName(controller));
	SDL_CloseGamepad(controller);
}

HapticsDevice* SDL3Controller::getHaptics() {
	return haptics.get();
}

ControllerEventType SDL3SystemAPI::getControllerEventType(SDL_Event& event) {
	switch (event.gbutton.type) {
	case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
		return ControllerEventType::ButtonDown;
	case SDL_EVENT_GAMEPAD_BUTTON_UP:
		return ControllerEventType::ButtonUp;
	case SDL_EVENT_GAMEPAD_ADDED:
		return ControllerEventType::Connected;
	case SDL_EVENT_GAMEPAD_REMOVED:
		return ControllerEventType::Disconnected;
	case SDL_EVENT_GAMEPAD_AXIS_MOTION:
		return ControllerEventType::AxisMoved;
	default:
		return ControllerEventType::Unknown;
	}
}

ControllerButton SDL3SystemAPI::getButton(SDL_Event& event) {
	switch (event.gbutton.button) {
	case SDL_GAMEPAD_BUTTON_SOUTH:
		return ControllerButton::A;
	case SDL_GAMEPAD_BUTTON_EAST:
		return ControllerButton::B;
	case SDL_GAMEPAD_BUTTON_WEST:
		return ControllerButton::X;
	case SDL_GAMEPAD_BUTTON_NORTH:
		return ControllerButton::Y;
	case SDL_GAMEPAD_BUTTON_LEFT_SHOULDER:
		return ControllerButton::LB;
	case SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER:
		return ControllerButton::RB;
	case SDL_GAMEPAD_BUTTON_DPAD_UP:
		return ControllerButton::DpadUp;
	case SDL_GAMEPAD_BUTTON_DPAD_DOWN:
		return ControllerButton::DpadDown;
	case SDL_GAMEPAD_BUTTON_DPAD_LEFT:
		return ControllerButton::DpadLeft;
	case SDL_GAMEPAD_BUTTON_DPAD_RIGHT:
		return ControllerButton::DpadRight;
	case SDL_GAMEPAD_BUTTON_LEFT_STICK:
		return ControllerButton::LeftStick;
	case SDL_GAMEPAD_BUTTON_RIGHT_STICK:
		return ControllerButton::RightStick;
	case SDL_GAMEPAD_BUTTON_START:
		return ControllerButton::Start;
	case SDL_GAMEPAD_BUTTON_BACK:
		return ControllerButton::Select;
	case SDL_GAMEPAD_BUTTON_GUIDE:
		return ControllerButton::Home;
	case SDL_GAMEPAD_BUTTON_MISC1:
		return ControllerButton::Misc;
	case SDL_GAMEPAD_BUTTON_TOUCHPAD:
		return ControllerButton::Touchpad;
	default:
		return ControllerButton::Unknown;
	}
}

ControllerAxis SDL3SystemAPI::getAxis(SDL_Event& event) {
	switch (event.gaxis.axis) {
	case SDL_GAMEPAD_AXIS_LEFTX:
		return ControllerAxis::LeftX;
	case SDL_GAMEPAD_AXIS_LEFTY:
		return ControllerAxis::LeftY;
	case SDL_GAMEPAD_AXIS_RIGHTX:
		return ControllerAxis::RightX;
	case SDL_GAMEPAD_AXIS_RIGHTY:
		return ControllerAxis::RightY;
	case SDL_GAMEPAD_AXIS_LEFT_TRIGGER:
		return ControllerAxis::LT;
	case SDL_GAMEPAD_AXIS_RIGHT_TRIGGER:
		return ControllerAxis::RT;
	default:
		return ControllerAxis::Unknown;
	}
}
