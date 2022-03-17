#include "input.h"
#include <growl/core/api/system_api.h>
#include <growl/core/log.h>

using Growl::InputHandler;

void InputHandler::onMouseEvent(InputMouseEvent& event) {
	mouseX = event.mouseX;
	mouseY = event.mouseY;
	system->log(
		LogLevel::DEBUG, "InputHandler", "Got mouse event! ({},{})", mouseX,
		mouseY);
}

void InputHandler::onKeyboardEvent(InputKeyboardEvent& event) {
	switch (event.type) {
	case KeyEventType::Unknown:
		system->log(LogLevel::DEBUG, "InputHandler", "Got unknown key event!");
		break;
	case KeyEventType::KeyDown:
		system->log(LogLevel::DEBUG, "InputHandler", "Got key down event!");
		break;
	case KeyEventType::KeyUp:
		system->log(LogLevel::DEBUG, "InputHandler", "Got key up event!");
		break;
	}
}
