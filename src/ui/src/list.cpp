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
		auto node = getChildren().at(i++).get();
		auto widget = nodeAsWidget(node);
		pack.prefWidthResult = pack.prefWidth.evaluate(node);
		pack.prefHeightResult = pack.prefHeight.evaluate(node);
		pack.resolvedWidth = widget && widget->getPrefWidth()
								 ? widget->getPrefWidth()
								 : pack.prefWidthResult;
		pack.resolvedHeight = widget && widget->getPrefHeight()
								  ? widget->getPrefHeight()
								  : pack.prefHeightResult;

		remaining -= vertical
						 ? pack.resolvedHeight + pack.marginTop.evaluate(node) +
							   pack.marginBottom.evaluate(node)
						 : pack.resolvedWidth + pack.marginLeft.evaluate(node) +
							   pack.marginRight.evaluate(node);

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
		auto node = getChildren().at(i++).get();
		switch (direction) {
		case Direction::VERTICAL: {
			node->setWidth(
				pack.fill			   ? max_width
				: pack.prefWidthResult ? pack.prefWidthResult
									   : pack.resolvedWidth);

			float margin_left = pack.marginLeft.evaluate(node);
			switch (pack.alignment) {
			case Align::START:
				node->setX(margin_left);
				break;
			case Align::MIDDLE:
				node->setX((max_width - node->getWidth()) / 2 + margin_left);
				break;
			case Align::END:
				node->setX((max_width - node->getWidth()) + margin_left);
				break;
			}

			position += pack.marginTop.evaluate(node);

			node->setY(position);
			float height = pack.resolvedHeight;
			if (pack.expand) {
				height += extra;
			}
			position += height + pack.marginBottom.evaluate(node);
			node->setHeight(height);
			break;
		}
		case Direction::HORIZONTAL: {
			node->setHeight(
				pack.fill				? max_height
				: pack.prefHeightResult ? pack.prefHeightResult
										: pack.resolvedHeight);

			float margin_top = pack.marginTop.evaluate(node);
			switch (pack.alignment) {
			case Align::START:
				node->setY(margin_top);
				break;
			case Align::MIDDLE:
				node->setY((max_height - node->getHeight()) / 2 + margin_top);
				break;
			case Align::END:
				node->setY((max_height - node->getHeight()) + margin_top);
				break;
			}

			position += pack.marginLeft.evaluate(node);

			node->setX(position);
			float width = pack.resolvedWidth;
			if (pack.expand) {
				width += extra;
			}
			position += width + pack.marginBottom.evaluate(node);
			node->setWidth(width);
			break;
		}
		}

		if (auto widget = nodeAsWidget(node)) {
			widget->invalidate();
			widget->validate();
		}
	}
}
