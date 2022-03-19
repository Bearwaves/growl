#pragma once

namespace Growl {

enum class ControllerEventType {
	Unknown,
	ButtonDown,
	ButtonUp,
	AxisMoved,
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

struct InputControllerEvent {
	ControllerEventType type;
	ControllerButton button;
};

} // namespace Growl
