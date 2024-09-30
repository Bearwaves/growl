#pragma once

#include "growl/ui/widget.h"

namespace Growl {

class List : public Widget {
public:
	enum class Direction { VERTICAL, HORIZONTAL };

	List(std::string&& name, Direction direction);
	void layout() override;

private:
	Direction direction;
};

} // namespace Growl