#pragma once

#include "growl/ui/widget.h"
namespace Growl {

class Value;

class Slider : public Widget {
public:
	Slider(std::string&& name, Value handle_width, Value handle_height);

	virtual void layout() override;

	virtual bool onMouseEvent(const InputMouseEvent& event) override;
	virtual bool onTouchEvent(const InputTouchEvent& event) override;

	float getValue() {
		return value;
	}
	float setValue(float value);

protected:
	float getHandleX() {
		return handle_x;
	}

	float getHandleY() {
		return handle_y;
	}

	float getHandleWidth() {
		return handle_w;
	}

	float getHandleHeight() {
		return handle_h;
	}

	bool isPointerDown() {
		return pointer_down;
	}

private:
	Value handle_width;
	Value handle_height;
	float handle_w = 0.f;
	float handle_h = 0.f;
	float handle_x = 0.f;
	float handle_y = 0.f;
	float value = 0.f;
	bool pointer_down = false;
};

} // namespace Growl
