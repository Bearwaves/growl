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
		if (pack.maxWidth) {
			pack.prefWidthResult =
				std::min(pack.prefWidthResult, pack.maxWidth.evaluate(node));
		}
		pack.prefHeightResult = pack.prefHeight.evaluate(node);
		if (pack.maxHeight) {
			pack.prefHeightResult =
				std::min(pack.prefHeightResult, pack.maxHeight.evaluate(node));
		}
		pack.resolvedWidth = widget && widget->getPrefWidth()
								 ? widget->getPrefWidth()
								 : pack.prefWidthResult;
		pack.resolvedHeight = widget && widget->getPrefHeight()
								  ? widget->getPrefHeight()
								  : pack.prefHeightResult;
		pack.boundsWidth = pack.prefWidthResult +
						   pack.marginLeft.evaluate(node) +
						   pack.marginRight.evaluate(node);
		pack.boundsHeight = pack.resolvedHeight +
							pack.marginTop.evaluate(node) +
							pack.marginBottom.evaluate(node);

		remaining -= vertical ? pack.boundsHeight : pack.boundsWidth;

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

		float margin_top = pack.marginTop.evaluate(node);
		float margin_bottom = pack.marginBottom.evaluate(node);
		float margin_left = pack.marginLeft.evaluate(node);
		float margin_right = pack.marginRight.evaluate(node);

		switch (direction) {
		case Direction::VERTICAL: {
			if (pack.expand) {
				pack.boundsHeight += extra;
			}

			node->setWidth(
				pack.fill_across ? max_width - (margin_left + margin_right)
				: pack.prefWidthResult ? pack.prefWidthResult
									   : pack.resolvedWidth);
			node->setHeight(
				pack.fill_along
					? pack.boundsHeight - (margin_top + margin_bottom)
					: pack.resolvedHeight);

			switch (pack.align_across) {
			case Align::LEFT:
				node->setX(margin_left);
				break;
			case Align::MIDDLE:
				node->setX(
					(max_width - node->getWidth()) / 2 + margin_left -
					margin_right);
				break;
			case Align::RIGHT:
				node->setX((max_width - node->getWidth()) + margin_left);
				break;
			default:
				break;
			}

			switch (pack.align_along) {
			case Align::TOP:
				node->setY(position + margin_top);
				break;
			case Align::MIDDLE:
				node->setY(
					position +
					((pack.boundsHeight + margin_top - margin_bottom) -
					 node->getHeightRaw()) /
						2);
				break;
			case Align::BOTTOM:
				node->setY(
					position + (pack.boundsHeight - node->getHeightRaw()) -
					margin_bottom);
				break;
			default:
				break;
			}

			position += pack.boundsHeight;

			break;
		}
		case Direction::HORIZONTAL: {
			if (pack.expand) {
				pack.boundsWidth += extra;
			}

			node->setWidth(
				pack.fill_along
					? pack.boundsWidth - (margin_left + margin_right)
					: pack.resolvedWidth);
			node->setHeight(
				pack.fill_across ? max_height - (margin_top + margin_bottom)
				: pack.prefHeightResult ? pack.prefHeightResult
										: pack.resolvedHeight);

			switch (pack.align_across) {
				// TODO fix these
			case Align::TOP:
				node->setY(margin_top);
				break;
			case Align::MIDDLE:
				node->setY(
					(max_height - node->getHeightRaw()) / 2 + margin_top -
					margin_bottom);
				break;
			case Align::BOTTOM:
				node->setY((max_height - node->getHeightRaw()) + margin_top);
				break;
			default:
				break;
			}

			switch (pack.align_along) {
			case Align::LEFT:
				node->setX(position + margin_left);
				break;
			case Align::MIDDLE:
				node->setX(
					position +
					((pack.boundsWidth + margin_left - margin_right) -
					 node->getWidthRaw()) /
						2);
				break;
			case Align::RIGHT:
				node->setX(
					position + (pack.boundsWidth - node->getWidthRaw()) -
					margin_right);
				break;
			default:
				break;
			}

			position += pack.boundsWidth;

			break;
		}
		}

		if (auto widget = nodeAsWidget(node)) {
			widget->invalidate();
			widget->validate();
		}
	}
}
