#include "input.h"
#include "growl/core/api/system_api.h"
#include "growl/core/input/event.h"
#include "growl/core/input/keyboard.h"
#include "growl/core/log.h"
#include "growl/scene/node.h"

using Growl::InputHandler;

bool InputHandler::onEvent(const InputEvent& event) {
	if (InputProcessor::onEvent(event)) {
		return true;
	}
	return root->onEvent(event);
}

bool InputHandler::onKeyboardEvent(const InputKeyboardEvent& event) {
	switch (event.type) {
	case KeyEventType::Unknown:
		system.log(LogLevel::Warn, "InputHandler", "Got unknown key event!");
		return false;
	case KeyEventType::KeyDown:
	case KeyEventType::KeyUp:
		switch (event.key) {
		case Key::ArrowUp:
		case Key::LetterW:
			up = event.type == KeyEventType::KeyDown;
			return true;
		case Key::ArrowDown:
		case Key::LetterS:
			down = event.type == KeyEventType::KeyDown;
			return true;
		case Key::ArrowLeft:
		case Key::LetterA:
			left = event.type == KeyEventType::KeyDown;
			return true;
		case Key::ArrowRight:
		case Key::LetterD:
			right = event.type == KeyEventType::KeyDown;
			return true;
		case Key::LetterQ:
			anticlockwise = event.type == KeyEventType::KeyDown;
			return true;
		case Key::LetterE:
			clockwise = event.type == KeyEventType::KeyDown;
			return true;
		default:
			return false;
		}
		return false;
	}
	return false;
}

bool InputHandler::onControllerEvent(const InputControllerEvent& event) {
	switch (event.type) {
	case ControllerEventType::Unknown:
		system.log(
			LogLevel::Warn, "InputHandler", "Got unknown controller event!");
		return false;
	case ControllerEventType::ButtonDown:
	case ControllerEventType::ButtonUp:
		switch (event.button) {
		case ControllerButton::DpadUp:
			up = event.type == ControllerEventType::ButtonDown;
			return true;
		case ControllerButton::DpadDown:
			down = event.type == ControllerEventType::ButtonDown;
			return true;
		case ControllerButton::DpadLeft:
			left = event.type == ControllerEventType::ButtonDown;
			return true;
		case ControllerButton::DpadRight:
			right = event.type == ControllerEventType::ButtonDown;
			return true;
		case ControllerButton::LB:
			anticlockwise = event.type == ControllerEventType::ButtonDown;
			return true;
		case ControllerButton::RB:
			clockwise = event.type == ControllerEventType::ButtonDown;
			return true;
		default:
			return false;
		}
		return false;
	default:
		return false;
	}
	return false;
}
