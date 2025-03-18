#include "growl/ui/slider.h"

using Growl::Slider;

Slider::Slider(
	std::string&& name, Value handle_width, Value handle_height,
	float initial_value, std::function<void(float)> on_value_changed)
	: Widget{std::move(name)}
	, handle_width{handle_width}
	, handle_height{handle_height}
	, value{initial_value}
	, on_value_changed{on_value_changed} {}

void Slider::layout() {
	handle_w = handle_width.evaluate(this);
	handle_h = handle_height.evaluate(this);
	handle_y = (getHeight() - handle_h) / 2;
	setValue(this->value);
}

float Slider::setValue(float value) {
	this->value = std::fmin(1, std::fmax(0, value));
	handle_x = (this->value * (getWidth() - handle_w));
	on_value_changed(this->value);
	return this->value;
}

bool Slider::onMouseEvent(const InputMouseEvent& event) {
	glm::vec4 internal_coordinates =
		worldToLocalCoordinates(event.mouseX, event.mouseY);
	switch (event.type) {
	case PointerEventType::Up:
		pointer_down = false;
		return false;
	case PointerEventType::Down:
		if (!hit(internal_coordinates)) {
			return false;
		}
		pointer_down = true;
	case PointerEventType::Move:
		if (!pointer_down) {
			return false;
		}
		setValue(
			(internal_coordinates.x - handle_w / 2) / (getWidth() - handle_w));
		return true;
	default:
		break;
	}
	return false;
}

bool Slider::onTouchEvent(const InputTouchEvent& event) {
	switch (event.type) {
	case PointerEventType::Up:
	case PointerEventType::Down:
	case PointerEventType::Move:
		break;
	default:
		break;
	}
	return false;
}
