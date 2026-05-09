#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "growl/core/input/processor.h"

using Growl::Controller;
using Growl::ControllerButton;
using Growl::InputControllerEvent;
using Growl::InputProcessor;

void Controller::tick(double delta_time, InputProcessor* processor) {
	for (auto& [button, time] : hold_times) {
		if (time > initial_repeat_delay + repeat_rate) {
			hold_times[button] = time - repeat_rate;
			if (processor) {
				InputEvent e{
					InputEventType::Controller,
					InputControllerEvent{
						getId(), ControllerEventType::ButtonDown, button}};
				processor->onEvent(e);
			}
		} else {
			hold_times[button] = time + delta_time;
		}
	}
}

bool Controller::onEvent(
	const InputControllerEvent& event, InputProcessor* processor) {
	switch (event.type) {
	case ControllerEventType::ButtonDown:
		if (repeatingButton(event.button)) {
			hold_times[event.button] = 0;
		}
		break;
	case ControllerEventType::ButtonUp:
		hold_times.erase(event.button);
		break;
	case ControllerEventType::AxisMoved:
		break;
	default:
		break;
	}
	if (processor) {
		return processor->onEvent(
			InputEvent{InputEventType::Controller, event});
	}
	return false;
}

bool Controller::repeatingButton(ControllerButton button) {
	switch (button) {
	case ControllerButton::DpadUp:
	case ControllerButton::DpadDown:
	case ControllerButton::DpadLeft:
	case ControllerButton::DpadRight:
		return true;
	default:
		return false;
	}
}
