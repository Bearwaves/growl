#pragma once

#include <unordered_map>

namespace Growl {

class HapticsDevice;

enum class ControllerLayout {
	BAYX, // e.g. Xbox
	ABXY, // e.g. Switch
	PS,	  // PlayStation reports buttons in BAYX layout
};

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
	int controller = 0;
	ControllerEventType type = ControllerEventType::Unknown;
	ControllerButton button = ControllerButton::Unknown;
	ControllerAxis axis = ControllerAxis::Unknown;
	float value = 0;
};

class InputProcessor;

class Controller {
public:
	Controller(float initial_repeat_delay = .2f, float repeat_rate = .1f)
		: initial_repeat_delay{initial_repeat_delay}
		, repeat_rate{repeat_rate} {}
	virtual ~Controller() = default;
	virtual HapticsDevice* getHaptics() = 0;
	virtual int getId() = 0;
	virtual ControllerLayout getLayout() = 0;

	virtual void tick(double delta_time, InputProcessor* processor);
	virtual bool
	onEvent(const InputControllerEvent& event, InputProcessor* processor);

protected:
	float initial_repeat_delay;
	float repeat_rate;
	std::unordered_map<ControllerButton, double> hold_times;

	bool repeatingButton(ControllerButton button);
};

} // namespace Growl
