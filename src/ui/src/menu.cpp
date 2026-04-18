#include "growl/ui/menu.h"
#include "growl/ui/menu_item.h"

using Growl::Menu;
using Growl::MenuItem;

MenuItem* Menu::getSelected() {
	return selected;
}

void Menu::setSelected(MenuItem* selected) {
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

void Menu::registerSelectable(MenuItem* item) {
	items.push_back(item);
}

void Menu::registerSelectable(std::vector<MenuItem*>& items) {
	this->items.insert(this->items.end(), items.begin(), items.end());
}

void Menu::setEdges(
	MenuItem* top, MenuItem* bottom, MenuItem* left, MenuItem* right) {
	this->top = top;
	this->bottom = bottom;
	this->leftmost = left;
	this->rightmost = right;
}

void Menu::linkSelectableVertically() {
	if (items.empty()) {
		return;
	}
	for (int i = 0; i < items.size(); i++) {
		items.at(i)->setDirections(
			i == 0 ? items.back() : items.at(i - 1),
			i == items.size() - 1 ? items.front() : items.at(i + 1));
	}
}

void Menu::linkSelectableHorizontally() {
	if (items.empty()) {
		return;
	}
	for (int i = 0; i < items.size(); i++) {
		items.at(i)->setDirections(
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
	if (selected && selected->up) {
		selected = selected->up;
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
	if (selected && selected->down) {
		selected = selected->down;
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
		if (selected->left) {
			selected = selected->left;
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
		if (selected && selected->right) {
			selected = selected->right;
		}
	} else if (leftmost) {
		selected = leftmost;
	} else {
		return;
	}
	selected->setSelected(true);
}
