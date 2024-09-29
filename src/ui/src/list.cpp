#include "growl/ui/list.h"
#include "growl/ui/widget.h"

using Growl::List;
using Growl::Node;

List::List(std::string&& name, Direction direction)
	: Widget{std::move(name)}
	, direction{direction} {}

void List::layout() {
	float size = direction == Direction::VERTICAL
					 ? getHeight() / getChildren().size()
					 : getWidth() / getChildren().size();
	int count = 0;
	for (auto& child : getChildren()) {
		switch (direction) {
		case Direction::VERTICAL:
			child->setWidth(getWidth());
			child->setHeight(size);
			child->setX(0);
			child->setY(size * count++);
			break;
		case Direction::HORIZONTAL:
			child->setWidth(size);
			child->setHeight(getHeight());
			child->setX(size * count++);
			child->setY(0);
			break;
		}
		if (nodeIsWidget(*child)) {
			static_cast<Widget&>(*child).invalidate();
		}
	}
}
