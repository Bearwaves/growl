#pragma once

#include <vector>

namespace Growl {

class Widget;

class Menu {

public:
	Widget* getSelection();
	virtual void setSelection(Widget* selected);
	virtual void deselect();
	virtual void clearSelectable();
	virtual void registerSelectable(Widget* item);
	virtual void registerSelectable(std::vector<Widget*>& items);
	std::vector<Widget*>& menuItems() {
		return items;
	}
	virtual void setEdges(
		Widget* top, Widget* bottom, Widget* left = nullptr,
		Widget* right = nullptr);
	virtual void linkSelectableVertically();
	virtual void linkSelectableHorizontally();

	virtual bool navigateUp();
	virtual bool navigateDown();
	virtual bool navigateLeft();
	virtual bool navigateRight();

private:
	Widget* selected = nullptr;
	Widget* top = nullptr;
	Widget* bottom = nullptr;
	Widget* leftmost = nullptr;
	Widget* rightmost = nullptr;
	std::vector<Widget*> items;
};

} // namespace Growl
