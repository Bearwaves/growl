#include "growl/ui/scrolling_menu.h"
#include "growl/ui/pack.h"
#include "growl/ui/scroll_pane.h"

using Growl::ScrollingMenu;
using Growl::ScrollPane;
using Growl::Value;
using Growl::Widget;

ScrollingMenu::ScrollingMenu(std::string&& name, Value threshold)
	: ScrollPane{std::move(name)}
	, threshold{threshold} {}

void ScrollingMenu::setSelection(Widget* selection) {
	Menu::setSelection(selection);
	scrollIfNeeded();
}

bool ScrollingMenu::navigateUp() {
	bool r = Menu::navigateUp();
	scrollIfNeeded();
	return r;
}

bool ScrollingMenu::navigateDown() {
	bool r = Menu::navigateDown();
	scrollIfNeeded();
	return r;
}

bool ScrollingMenu::navigateLeft() {
	bool r = Menu::navigateLeft();
	scrollIfNeeded();
	return r;
}

bool ScrollingMenu::navigateRight() {
	bool r = Menu::navigateRight();
	scrollIfNeeded();
	return r;
}

void ScrollingMenu::scrollIfNeeded() {
	if (!getSelection()) {
		return;
	}

	Widget* item = getSelection();
	if (!item->isDescendantOf(this)) {
		return;
	}
	float threshold_v = this->threshold.evaluate(this);

	glm::vec4 world_coords = item->localToWorldCoordinates(0, 0);
	glm::vec4 scroll_coords =
		worldToLocalCoordinates(world_coords.x, world_coords.y);

	float left = scroll_coords.x;
	float right = (scroll_coords.x + item->getWidth()) - getWidth();
	float top = scroll_coords.y;
	float bottom = (scroll_coords.y + item->getHeight()) - getHeight();

	if (left < threshold_v) {
		setScrollX(getScrollX() - (left - threshold_v));
	} else if (right > -threshold_v) {
		setScrollX(getScrollX() - (right + threshold_v));
	}

	if (top < threshold_v) {
		setScrollY(getScrollY() + top - threshold_v);
	} else if (bottom > -threshold_v) {
		setScrollY(getScrollY() + bottom + threshold_v);
	}
}
