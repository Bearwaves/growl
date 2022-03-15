#include "../../include/growl/core/input/processor.h"
#include <iostream>

using Growl::InputEvent;
using Growl::InputProcessor;

// Define pure virtual destructor to make class abstract
// without any pure virtual functions.
InputProcessor::~InputProcessor() {}

void InputProcessor::onEvent(InputEvent& event) {
	switch (event.getType()) {
	case InputEventType::KEYBOARD: {
		auto keyboardEventResult = event.getEvent<InputKeyboardEvent>();
		if (keyboardEventResult.hasResult()) {
			onKeyboardEvent(keyboardEventResult.get());
		}
		return;
	}
	case InputEventType::MOUSE: {
		auto mouseEventResult = event.getEvent<InputMouseEvent>();
		if (mouseEventResult.hasResult()) {
			onMouseEvent(mouseEventResult.get());
		}
		return;
	}
	default:
		return;
	}
}
