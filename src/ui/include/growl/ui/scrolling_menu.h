#pragma once

#include "growl/ui/menu.h"
#include "growl/ui/scroll_pane.h"

namespace Growl {

class ScrollingMenu : public Growl::ScrollPane, public Growl::Menu {
public:
	explicit ScrollingMenu(std::string&& name, Growl::Value threshold = 0);

	void setSelection(Widget* selection) override;
	void navigateUp() override;
	void navigateDown() override;
	void navigateLeft() override;
	void navigateRight() override;

private:
	Growl::Value threshold;

	void scrollIfNeeded();
};

} // namespace Growl
