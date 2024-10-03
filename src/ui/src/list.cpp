#include "growl/ui/list.h"
#include "growl/ui/widget.h"

using Growl::List;
using Growl::Node;

void List::layout() {
	int expands = 0;
	bool vertical = direction == Direction::VERTICAL;
	float max_width = getWidth();
	float max_height = getHeight();
	float remaining = vertical ? max_height : max_width;

	// First pass, work out how much extra room we have to play with.
	// Resolve any nodes that already have a height or width (e.g. text labels).
	int i = 0;
	for (auto& pack : pack_info) {
		auto& node = getChildren().at(i++);
		if (auto w = node->getWidth(); w > pack.prefWidth) {
			pack.prefWidth = w;
		}
		if (auto h = node->getHeight(); h > pack.prefHeight) {
			pack.prefHeight = h;
		}
		remaining -= vertical ? pack.prefHeight : pack.prefWidth;

		if (pack.expand) {
			expands++;
		}
	}

	// Divide the remaining room amongst the expanding children and assign
	// sizes and positions accordingly.
	float extra = expands > 0 ? remaining / expands : 0;
	float position = 0;
	i = 0;
	for (auto& pack : pack_info) {
		auto& node = getChildren().at(i++);
		switch (direction) {
		case Direction::VERTICAL: {
			node->setWidth(pack.fill ? max_width : pack.prefWidth);

			switch (pack.alignment) {
			case Align::START:
				node->setX(0);
				break;
			case Align::MIDDLE:
				node->setX((max_width - node->getWidth()) / 2);
				break;
			case Align::END:
				node->setX(max_width - node->getWidth());
				break;
			}

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
			node->setHeight(pack.fill ? max_height : pack.prefHeight);

			switch (pack.alignment) {
			case Align::START:
				node->setY(0);
				break;
			case Align::MIDDLE:
				node->setY((max_height - node->getHeight()) / 2);
				break;
			case Align::END:
				node->setY(max_height - node->getHeight());
				break;
			}

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

		if (auto widget = nodeAsWidget(node.get())) {
			widget->invalidate();
		}
	}
}
