#pragma once

namespace Growl {

class Menu;

class MenuItem {

	friend class Menu;

public:
	virtual void setDirections(
		MenuItem* up, MenuItem* down, MenuItem* left = nullptr,
		MenuItem* right = nullptr) {
		this->up = up;
		this->down = down;
		this->left = left;
		this->right = right;
	}

protected:
	virtual void setSelected(bool selected) {
		this->selected = selected;
	}
	bool selected = false;

	MenuItem* up = nullptr;
	MenuItem* down = nullptr;
	MenuItem* left = nullptr;
	MenuItem* right = nullptr;
};

} // namespace Growl
