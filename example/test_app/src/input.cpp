#include "input.h"
#include <growl/core/api/system_api.h>
#include <growl/core/log.h>

using Growl::InputHandler;

void InputHandler::onMouseEvent(InputMouseEvent& event) {
	mouseX = event.mouseX;
	mouseY = event.mouseY;
}

void InputHandler::onKeyboardEvent(InputKeyboardEvent& event) {
	switch (event.type) {
	case KeyEventType::Unknown:
		system->log(LogLevel::WARN, "InputHandler", "Got unknown key event!");
		break;
	case KeyEventType::KeyDown:
	case KeyEventType::KeyUp:
		switch (event.key) {
		case Key::ArrowUp:
		case Key::LetterW:
			up = event.type == KeyEventType::KeyDown;
			break;
		case Key::ArrowDown:
		case Key::LetterS:
			down = event.type == KeyEventType::KeyDown;
			break;
		case Key::ArrowLeft:
		case Key::LetterA:
			left = event.type == KeyEventType::KeyDown;
			break;
		case Key::ArrowRight:
		case Key::LetterD:
			right = event.type == KeyEventType::KeyDown;
			break;
		default:
			break;
		}
		break;
	}
}

void InputHandler::onControllerEvent(InputControllerEvent& event) {
	switch (event.type) {
	case ControllerEventType::Unknown:
		system->log(
			LogLevel::WARN, "InputHandler", "Got unknown controller event!");
		break;
	case ControllerEventType::ButtonDown:
	case ControllerEventType::ButtonUp:
		switch (event.button) {
		case ControllerButton::DpadUp:
			up = event.type == ControllerEventType::ButtonDown;
			break;
		case ControllerButton::DpadDown:
			down = event.type == ControllerEventType::ButtonDown;
			break;
		case ControllerButton::DpadLeft:
			left = event.type == ControllerEventType::ButtonDown;
			break;
		case ControllerButton::DpadRight:
			right = event.type == ControllerEventType::ButtonDown;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
