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
	virtual void onEvent(InputEvent& event);

	virtual void onKeyboardEvent(InputKeyboardEvent& event) {}
	virtual void onMouseEvent(InputMouseEvent& event) {}
	virtual void onTouchEvent(InputTouchEvent& event) {}
	virtual void onControllerEvent(InputControllerEvent& event) {}
	virtual void onCustomEvent(InputCustomEvent& event) {}
};

} // namespace Growl
