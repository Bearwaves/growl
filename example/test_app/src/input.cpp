#include "input.h"
#include "growl/core/api/system_api.h"

using Growl::InputHandler;

void InputHandler::onMouseEvent(InputMouseEvent& event) {
	mouseX = event.getMouseX();
	mouseY = event.getMouseY();
	system->log(
		LogLevel::DEBUG, "InputHandler", "Got mouse event! ({},{})", mouseX,
		mouseY);
}
