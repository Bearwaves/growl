#pragma once

namespace Growl {

enum class ControllerEventType {
	Unknown,
	ButtonDown,
	ButtonUp,
	AxisMoved,
	Connected,
	Disconnected,
};

enum class ControllerButton {
	Unknown,

	A,
	B,
	X,
	Y,

	DpadUp,
	DpadDown,
	DpadLeft,
	DpadRight,

	LT,
	RT,
	LB,
	RB,

	Start,
	Select,
	Home,
	Misc,
	Touchpad,

	LeftStick,
	RightStick,
};

enum class ControllerAxis {
	Unknown,

	LeftX,
	LeftY,
	RightX,
	RightY,

	LT,
	RT,
};

struct InputControllerEvent {
	ControllerEventType type = ControllerEventType::Unknown;
	ControllerButton button = ControllerButton::Unknown;
	ControllerAxis axis = ControllerAxis::Unknown;
	float value = 0;
};

} // namespace Growl
