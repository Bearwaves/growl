#pragma once

#include <vector>

namespace Growl {

class MenuItem;

class Menu {

	friend class MenuItem;

public:
	MenuItem* getSelected();
	virtual void setSelected(MenuItem* selected);
	virtual void deselect();
	virtual void clearSelectable();
	virtual void registerSelectable(MenuItem* item);
	virtual void registerSelectable(std::vector<MenuItem*>& items);
	std::vector<MenuItem*>& menuItems() {
		return items;
	}
	virtual void setEdges(
		MenuItem* top, MenuItem* bottom, MenuItem* left = nullptr,
		MenuItem* right = nullptr);
	virtual void linkSelectableVertically();
	virtual void linkSelectableHorizontally();

	virtual void navigateUp();
	virtual void navigateDown();
	virtual void navigateLeft();
	virtual void navigateRight();

private:
	MenuItem* selected = nullptr;
	MenuItem* top = nullptr;
	MenuItem* bottom = nullptr;
	MenuItem* leftmost = nullptr;
	MenuItem* rightmost = nullptr;
	std::vector<MenuItem*> items;
};

} // namespace Growl
