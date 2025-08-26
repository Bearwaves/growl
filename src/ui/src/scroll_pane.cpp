#include "growl/ui/scroll_pane.h"
#include "growl/core/graphics/batch.h"
#include "growl/core/input/event.h"
#include "growl/ui/widget.h"
#include <cmath>

using Growl::Batch;
using Growl::InputMouseEvent;
using Growl::ScrollPane;

void ScrollPane::layout() {
	int i = 0;
	for (auto& pack : pack_info) {
		auto node = getChildren().at(i++);
		auto widget = nodeAsWidget(node);

		pack.prefWidthResult = pack.prefWidth.evaluate(node);
		if (pack.maxWidth) {
			pack.prefWidthResult =
				std::min(pack.prefWidthResult, pack.maxWidth.evaluate(node));
		}
		pack.prefHeightResult = pack.prefHeight.evaluate(node);
		if (pack.maxHeight) {
			pack.prefHeightResult =
				std::min(pack.prefHeightResult, pack.maxHeight.evaluate(node));
		}
		pack.resolvedWidth = widget && widget->getPrefWidth()
								 ? widget->getPrefWidth()
								 : pack.prefWidthResult;
		pack.resolvedHeight = widget && widget->getPrefHeight()
								  ? widget->getPrefHeight()
								  : pack.prefHeightResult;

		node->setWidth(pack.resolvedWidth);
		node->setHeight(pack.resolvedHeight);
		if (widget) {
			widget->invalidate();
			widget->validate();
		}

		float margin_top = pack.marginTop.evaluate(node);
		float margin_bottom = pack.marginBottom.evaluate(node);
		float margin_left = pack.marginLeft.evaluate(node);
		float margin_right = pack.marginRight.evaluate(node);

		min_x = getWidth() - (node->getWidth() + margin_right);
		max_x = margin_left;
		min_y = getHeight() - (node->getHeight() + margin_bottom);
		max_y = margin_top;
		node->setX(clampX(node->getX()));
		node->setY(clampY(node->getY()));
	}
}

void ScrollPane::onDraw(
	Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	batch.setScissor(0, 0, getWidth(), getHeight(), transform);
	Widget::onDraw(batch, parent_alpha, transform);
	batch.resetScissor();
}

bool ScrollPane::onMouseEvent(const InputMouseEvent& event) {
	// Block down events that are outside the scroll pane
	return event.type == PointerEventType::Down &&
		   !hit(event.mouseX, event.mouseY);
}

bool ScrollPane::onTouchEvent(const InputTouchEvent& event) {
	return event.type == PointerEventType::Down &&
		   !hit(event.touchX, event.touchY);
}

bool ScrollPane::onPostEvent(const InputEvent& event, bool children_handled) {
	switch (event.getType()) {
	case InputEventType::Mouse: {
		auto mouse_event_result = event.getEvent<InputMouseEvent>();
		if (mouse_event_result.hasResult()) {
			return onMouseEventPost(mouse_event_result.get(), children_handled);
		}
		return children_handled;
	}
	case InputEventType::Touch: {
		auto touch_event_result = event.getEvent<InputTouchEvent>();
		if (touch_event_result.hasResult()) {
			return onTouchEventPost(touch_event_result.get(), children_handled);
		}
		return children_handled;
	}
	default:
		return children_handled;
	}
}

bool ScrollPane::onMouseEventPost(
	const InputMouseEvent& event, bool child_handled) {
	switch (event.type) {
	case PointerEventType::Scroll:
		if (!panning) {
			for (auto child : getChildren()) {
				child->setX(clampX(child->getX() + 20 * event.scrollX));
				child->setY(clampY(child->getY() + 20 * event.scrollY));
			}
		}
		break;
	case PointerEventType::Down:
		if (hit(event.mouseX, event.mouseY)) {
			panning = true;
			last_pointer_x = event.mouseX;
			last_pointer_y = event.mouseY;
		}
		break;
	case PointerEventType::Up:
		panning = false;
		break;
	case PointerEventType::Move:
		if (!child_handled) {
			pan(event.mouseX, event.mouseY);
		}
		break;
	default:
		break;
	}
	return child_handled;
}

bool ScrollPane::onTouchEventPost(
	const InputTouchEvent& event, bool child_handled) {
	switch (event.type) {
	case PointerEventType::Up:
		panning = false;
		break;
	case PointerEventType::Down:
		if (hit(event.touchX, event.touchY)) {
			panning = true;
			last_pointer_x = event.touchX;
			last_pointer_y = event.touchY;
		}
		break;
	case PointerEventType::Move:
		if (!child_handled) {
			pan(event.touchX, event.touchY);
		}
		break;
	default:
		break;
	}
	return child_handled;
}

float ScrollPane::getScrollX() {
	for (auto child : getChildren()) {
		return child->getX();
	}
	return 0;
}

float ScrollPane::getScrollY() {
	for (auto child : getChildren()) {
		return -child->getY();
	}
	return 0;
}

void ScrollPane::setScrollX(float x) {
	for (auto child : getChildren()) {
		child->setX(clampX(x));
	}
}

void ScrollPane::setScrollY(float y) {
	for (auto child : getChildren()) {
		child->setY(clampY(-y));
	}
}

float ScrollPane::getMaxX() {
	return max_x;
}

float ScrollPane::getMaxY() {
	return -min_y;
}

void ScrollPane::pan(float x, float y) {
	if (panning) {
		float diff_x = x - last_pointer_x;
		float diff_y = y - last_pointer_y;
		last_pointer_x = x;
		last_pointer_y = y;
		for (auto child : getChildren()) {
			child->setX(clampX(child->getX() + diff_x));
			child->setY(clampY(child->getY() + diff_y));
		}
	}
}

float ScrollPane::clampX(float x) {
	return std::fmin(std::fmax(x, min_x), max_x);
}

float ScrollPane::clampY(float y) {
	return std::fmin(std::fmax(y, min_y), max_y);
}
