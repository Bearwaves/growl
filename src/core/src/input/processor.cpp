#include "growl/core/input/processor.h"
#include <iostream>

using Growl::InputEvent;
using Growl::InputProcessor;

// Define pure virtual destructor to make class abstract
// without any pure virtual functions.
InputProcessor::~InputProcessor() {}

void InputProcessor::onEvent(InputEvent& event) {
	switch (event.getType()) {
	case InputEventType::KEYBOARD: {
		auto keyboard_event_result = event.getEvent<InputKeyboardEvent>();
		if (keyboard_event_result.hasResult()) {
			onKeyboardEvent(keyboard_event_result.get());
		}
		return;
	}
	case InputEventType::MOUSE: {
		auto mouse_event_result = event.getEvent<InputMouseEvent>();
		if (mouse_event_result.hasResult()) {
			onMouseEvent(mouse_event_result.get());
		}
		return;
	}
	default:
		return;
	}
}
