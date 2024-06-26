#include "growl/core/input/processor.h"
#include "growl/core/input/event.h"
#include "growl/core/input/keyboard.h"

using Growl::InputEvent;
using Growl::InputEventType;
using Growl::InputProcessor;

// Define pure virtual destructor to make class abstract
// without any pure virtual functions.
InputProcessor::~InputProcessor() {}

bool InputProcessor::onEvent(const InputEvent& event) {
	switch (event.getType()) {
	case InputEventType::Keyboard: {
		auto keyboard_event_result = event.getEvent<InputKeyboardEvent>();
		if (keyboard_event_result.hasResult()) {
			return onKeyboardEvent(keyboard_event_result.get());
		}
		return false;
	}
	case InputEventType::Mouse: {
		auto mouse_event_result = event.getEvent<InputMouseEvent>();
		if (mouse_event_result.hasResult()) {
			return onMouseEvent(mouse_event_result.get());
		}
		return false;
	}
	case InputEventType::Controller: {
		auto controller_event_result = event.getEvent<InputControllerEvent>();
		if (controller_event_result.hasResult()) {
			return onControllerEvent(controller_event_result.get());
		}
		return false;
	}
	case InputEventType::Touch: {
		auto touch_event_result = event.getEvent<InputTouchEvent>();
		if (touch_event_result.hasResult()) {
			return onTouchEvent(touch_event_result.get());
		}
		return false;
	}
	default:
		return false;
	}
}
