#pragma once

namespace Growl {

class InputEvent;
struct InputKeyboardEvent;
struct InputMouseEvent;
struct InputTouchEvent;
struct InputControllerEvent;
struct InputCustomEvent;

class InputProcessor {
public:
	virtual ~InputProcessor() = 0;
	virtual bool onEvent(const InputEvent& event);

protected:
	virtual bool onKeyboardEvent(const InputKeyboardEvent& event) {
		return false;
	}
	virtual bool onMouseEvent(const InputMouseEvent& event) {
		return false;
	}
	virtual bool onTouchEvent(const InputTouchEvent& event) {
		return false;
	}
	virtual bool onControllerEvent(const InputControllerEvent& event) {
		return false;
	}
	virtual bool onCustomEvent(const InputCustomEvent& event) {
		return false;
	}
};

} // namespace Growl
