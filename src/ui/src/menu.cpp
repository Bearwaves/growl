#include "growl/ui/menu.h"
#include "growl/ui/widget.h"

using Growl::Menu;
using Growl::Widget;

Widget* Menu::getSelection() {
	return selected;
}

void Menu::setSelection(Widget* selected) {
	deselect();
	this->selected = selected;
	if (this->selected) {
		this->selected->setSelected(true);
	}
}

void Menu::deselect() {
	if (this->selected) {
		this->selected->setSelected(false);
	}
	this->selected = nullptr;
}

void Menu::clearSelectable() {
	selected = nullptr;
	setEdges(nullptr, nullptr, nullptr, nullptr);
	items.clear();
}

void Menu::registerSelectable(Widget* item) {
	items.push_back(item);
}

void Menu::registerSelectable(std::vector<Widget*>& items) {
	this->items.insert(this->items.end(), items.begin(), items.end());
}

void Menu::setEdges(Widget* top, Widget* bottom, Widget* left, Widget* right) {
	this->top = top;
	this->bottom = bottom;
	this->leftmost = left;
	this->rightmost = right;
}

void Menu::linkSelectableVertically() {
	if (items.empty()) {
		return;
	}
	for (size_t i = 0; i < items.size(); i++) {
		items.at(i)->setNavigationDirections(
			i == 0 ? items.back() : items.at(i - 1),
			i == items.size() - 1 ? items.front() : items.at(i + 1));
	}
}

void Menu::linkSelectableHorizontally() {
	if (items.empty()) {
		return;
	}
	for (size_t i = 0; i < items.size(); i++) {
		items.at(i)->setNavigationDirections(
			nullptr, nullptr, i == 0 ? items.back() : items.at(i - 1),
			i == items.size() - 1 ? items.front() : items.at(i + 1));
	}
}

void Menu::navigateUp() {
	if (items.empty()) {
		return;
	}
	if (selected) {
		selected->setSelected(false);
	}
	if (selected && selected->up_nav) {
		selected = selected->up_nav;
	} else if (bottom) {
		selected = bottom;
	} else {
		selected = items.back();
	}
	selected->setSelected(true);
}

void Menu::navigateDown() {
	if (items.empty()) {
		return;
	}
	if (selected) {
		selected->setSelected(false);
	}
	if (selected && selected->down_nav) {
		selected = selected->down_nav;
	} else if (top) {
		selected = top;
	} else {
		selected = items.front();
	}
	selected->setSelected(true);
}

void Menu::navigateLeft() {
	if (items.empty()) {
		return;
	}
	if (selected) {
		selected->setSelected(false);
		if (selected->left_nav) {
			selected = selected->left_nav;
		}
	} else if (rightmost) {
		selected = rightmost;
	} else {
		return;
	}
	selected->setSelected(true);
}

void Menu::navigateRight() {
	if (items.empty()) {
		return;
	}
	if (selected) {
		selected->setSelected(false);
		if (selected && selected->right_nav) {
			selected = selected->right_nav;
		}
	} else if (leftmost) {
		selected = leftmost;
	} else {
		return;
	}
	selected->setSelected(true);
}
