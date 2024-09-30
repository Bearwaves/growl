#include "growl/ui/list.h"
#include "growl/ui/widget.h"

using Growl::List;
using Growl::Node;

List::List(std::string&& name, Direction direction)
	: Widget{std::move(name)}
	, direction{direction} {}

void List::layout() {
	int expands = 0;
	bool vertical = direction == Direction::VERTICAL;
	float max_width = getWidth();
	float max_height = getHeight();
	float remaining = vertical ? max_height : max_width;

	// First pass, work out how much extra room we have to play with.
	for (auto& pack : pack_info) {
		if (pack.expand) {
			expands++;
		} else {
			remaining -= vertical ? pack.prefHeight : pack.prefWidth;
		}
	}

	// Divide the remaining room amongst the expanding children and assign
	// sizes and positions accordingly.
	float extra = expands > 0 ? remaining / expands : 0;
	float position = 0;
	int i = 0;
	for (auto& pack : pack_info) {
		auto& node = getChildren().at(i++);
		switch (direction) {
		case Direction::VERTICAL: {
			node->setX(0);
			node->setWidth(pack.fill ? max_width : pack.prefWidth);
			node->setY(position);
			float height = pack.prefHeight;
			if (pack.expand) {
				height += extra;
			}
			position += height;
			node->setHeight(height);
			break;
		}
		case Direction::HORIZONTAL: {
			node->setY(0);
			node->setHeight(pack.fill ? max_height : pack.prefHeight);
			node->setX(position);
			float width = pack.prefWidth;
			if (pack.expand) {
				width += extra;
			}
			position += width;
			node->setWidth(width);
			break;
		}
		}

		if (nodeIsWidget(*node)) {
			static_cast<Widget&>(*node).invalidate();
		}
	}
}
