#include "growl/core/input/controller.h"
#include "growl/core/input/event.h"
#include "growl/core/input/processor.h"

using Growl::Controller;
using Growl::ControllerButton;
using Growl::InputControllerEvent;
using Growl::InputProcessor;

constexpr float STICK_DIRECTION_TOLERANCE = 0.6f;

void Controller::tick(double delta_time, InputProcessor* processor) {
	for (auto& [button, time] : hold_times) {
		if (time > initial_repeat_delay + repeat_rate) {
			hold_times[button] = time - repeat_rate;
			sendButtonEvent(processor, button);
		} else {
			hold_times[button] = time + delta_time;
		}
	}
	for (auto& [button, time] : stick_hold_times) {
		if (time > initial_repeat_delay + repeat_rate) {
			stick_hold_times[button] = time - repeat_rate;
			sendButtonEvent(processor, button);
		} else {
			stick_hold_times[button] = time + delta_time;
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
		if (processor &&
			processor->onEvent(InputEvent{InputEventType::Controller, event})) {
			// Axis event handled by game.
			return true;
		}
		switch (event.axis) {
		case ControllerAxis::LeftX:
			if (event.value > STICK_DIRECTION_TOLERANCE) {
				if (stick_hold_times.erase(ControllerButton::DpadLeft)) {
					sendButtonEvent(
						processor, ControllerButton::DpadLeft,
						ControllerEventType::ButtonUp);
				}
				if (!stick_hold_times.count(ControllerButton::DpadRight)) {
					stick_hold_times[ControllerButton::DpadRight] = 0.f;
					sendButtonEvent(processor, ControllerButton::DpadRight);
				}
			} else if (event.value < -STICK_DIRECTION_TOLERANCE) {
				if (stick_hold_times.erase(ControllerButton::DpadRight)) {
					sendButtonEvent(
						processor, ControllerButton::DpadRight,
						ControllerEventType::ButtonUp);
				}
				if (!stick_hold_times.count(ControllerButton::DpadLeft)) {
					stick_hold_times[ControllerButton::DpadLeft] = 0.f;
					sendButtonEvent(processor, ControllerButton::DpadLeft);
				}
			} else {
				if (stick_hold_times.erase(ControllerButton::DpadLeft)) {
					sendButtonEvent(
						processor, ControllerButton::DpadLeft,
						ControllerEventType::ButtonUp);
				}
				if (stick_hold_times.erase(ControllerButton::DpadRight)) {
					sendButtonEvent(
						processor, ControllerButton::DpadRight,
						ControllerEventType::ButtonUp);
				}
			}
			break;
		case ControllerAxis::LeftY:
			if (event.value > STICK_DIRECTION_TOLERANCE) {
				if (stick_hold_times.erase(ControllerButton::DpadUp)) {
					sendButtonEvent(
						processor, ControllerButton::DpadUp,
						ControllerEventType::ButtonUp);
				}
				if (!stick_hold_times.count(ControllerButton::DpadDown)) {
					stick_hold_times[ControllerButton::DpadDown] = 0.f;
					sendButtonEvent(processor, ControllerButton::DpadDown);
				}
			} else if (event.value < -STICK_DIRECTION_TOLERANCE) {
				if (stick_hold_times.erase(ControllerButton::DpadDown)) {
					sendButtonEvent(
						processor, ControllerButton::DpadDown,
						ControllerEventType::ButtonUp);
				}
				if (!stick_hold_times.count(ControllerButton::DpadUp)) {
					stick_hold_times[ControllerButton::DpadUp] = 0.f;
					sendButtonEvent(processor, ControllerButton::DpadUp);
				}
			} else {
				if (stick_hold_times.erase(ControllerButton::DpadUp)) {
					sendButtonEvent(
						processor, ControllerButton::DpadUp,
						ControllerEventType::ButtonUp);
				}
				if (stick_hold_times.erase(ControllerButton::DpadDown)) {
					sendButtonEvent(
						processor, ControllerButton::DpadDown,
						ControllerEventType::ButtonUp);
				}
			}
			break;
		default:
			break;
		}
		return false;
	default:
		break;
	}
	if (processor) {
		return processor->onEvent(
			InputEvent{InputEventType::Controller, event});
	}
	return false;
}

void Controller::clearHoldData() {
	hold_times.clear();
	stick_hold_times.clear();
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

bool Controller::sendButtonEvent(
	InputProcessor* processor, ControllerButton button,
	ControllerEventType type) {
	if (!processor) {
		return false;
	}
	InputEvent e{
		InputEventType::Controller,
		InputControllerEvent{getId(), type, button}};
	return processor->onEvent(e);
}
