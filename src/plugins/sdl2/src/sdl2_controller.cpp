#include "SDL_events.h"
#include "SDL_gamecontroller.h"
#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "sdl2_system.h"

using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::SDL2SystemAPI;

void SDL2SystemAPI::openGameController(int id) {
	auto sdl_controller = SDL_GameControllerOpen(id);
	log("SDL2SystemAPI", "Got controller: {}",
		SDL_GameControllerName(sdl_controller));
	if (SDL_GameControllerHasRumble(sdl_controller)) {
		log("SDL2SystemAPI", "Game controller has rumble available");
	}
	controller = std::make_unique<SDL2Controller>(this, sdl_controller);
}

void SDL2SystemAPI::handleControllerEvent(SDL_Event& event) {
	InputEvent e{
		InputEventType::Controller,
		InputControllerEvent{getControllerEventType(event), getButton(event)}};
	inputProcessor->onEvent(e);
}

ControllerEventType SDL2SystemAPI::getControllerEventType(SDL_Event& event) {
	switch (event.cbutton.state) {
	case SDL_PRESSED:
		return ControllerEventType::ButtonDown;
	case SDL_RELEASED:
		return ControllerEventType::ButtonUp;
	default:
		return ControllerEventType::Unknown;
	}
}

ControllerButton SDL2SystemAPI::getButton(SDL_Event& event) {
	switch (event.cbutton.button) {
	case SDL_CONTROLLER_BUTTON_A:
		return ControllerButton::A;
	case SDL_CONTROLLER_BUTTON_B:
		return ControllerButton::B;
	case SDL_CONTROLLER_BUTTON_X:
		return ControllerButton::X;
	case SDL_CONTROLLER_BUTTON_Y:
		return ControllerButton::Y;
	case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
		return ControllerButton::LB;
	case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
		return ControllerButton::RB;
	case SDL_CONTROLLER_BUTTON_DPAD_UP:
		return ControllerButton::DpadUp;
	case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
		return ControllerButton::DpadDown;
	case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
		return ControllerButton::DpadLeft;
	case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
		return ControllerButton::DpadRight;
	case SDL_CONTROLLER_BUTTON_LEFTSTICK:
		return ControllerButton::LeftStick;
	case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
		return ControllerButton::RightStick;
	case SDL_CONTROLLER_BUTTON_START:
		return ControllerButton::Start;
	case SDL_CONTROLLER_BUTTON_BACK:
		return ControllerButton::Select;
	case SDL_CONTROLLER_BUTTON_GUIDE:
		return ControllerButton::Home;
	case SDL_CONTROLLER_BUTTON_MISC1:
		return ControllerButton::Misc;
	case SDL_CONTROLLER_BUTTON_TOUCHPAD:
		return ControllerButton::Touchpad;
	default:
		return ControllerButton::Unknown;
	}
}
