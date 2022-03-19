#include "sdl_system.h"
#include <SDL.h>
#include <growl/core/input/event.h>

using Growl::ControllerButton;
using Growl::ControllerEventType;
using Growl::SDL2SystemAPI;

void SDL2SystemAPI::handleControllerEvent(SDL_Event& event) {
	InputControllerEvent e{getControllerEventType(event), getButton(event)};
	inputProcessor->onControllerEvent(e);
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
