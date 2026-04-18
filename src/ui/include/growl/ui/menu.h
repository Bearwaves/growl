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
	virtual void clearItems();
	virtual void addItem(MenuItem* item);
	virtual void addItems(std::vector<MenuItem*>& items);
	std::vector<MenuItem*>& menuItems() {
		return items;
	}
	virtual void setEdges(
		MenuItem* top, MenuItem* bottom, MenuItem* left = nullptr,
		MenuItem* right = nullptr);
	virtual void joinItemsVertically();

	virtual void up();
	virtual void down();
	virtual void left();
	virtual void right();

private:
	MenuItem* selected = nullptr;
	MenuItem* top = nullptr;
	MenuItem* bottom = nullptr;
	MenuItem* leftmost = nullptr;
	MenuItem* rightmost = nullptr;
	std::vector<MenuItem*> items;
};

} // namespace Growl
