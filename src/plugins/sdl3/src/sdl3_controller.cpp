#include "SDL3/SDL_events.h"
#include "SDL3/SDL_gamepad.h"
#include "growl/core/haptics.h"
#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "sdl3_haptics.h"
#include "sdl3_system.h"

using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::HapticsDevice;
using Growl::SDL3Controller;
using Growl::SDL3SystemAPI;

void SDL3SystemAPI::openGameController(int id) {
	auto sdl_controller = SDL_OpenGamepad(id);
	log("SDL3SystemAPI", "Got controller: {}",
		SDL_GetGamepadName(sdl_controller));
	controller = std::make_unique<SDL3Controller>(this, sdl_controller);
}

void SDL3SystemAPI::handleControllerEvent(SDL_Event& event) {
	InputEvent e{
		InputEventType::Controller,
		InputControllerEvent{getControllerEventType(event), getButton(event)}};
	inputProcessor->onEvent(e);
}

SDL3Controller::SDL3Controller(SystemAPI* system, SDL_Gamepad* controller)
	: system{system}
	, controller{controller}
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
