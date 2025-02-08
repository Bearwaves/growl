#pragma once

#include "growl/ui/widget.h"

namespace Growl {

class Container : public Growl::Widget {
public:
	Container(std::string&& label)
		: Widget{std::move(label)} {}
	void layout() override;
};

} // namespace Growl
