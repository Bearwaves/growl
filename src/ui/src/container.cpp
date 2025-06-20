#include "growl/ui/container.h"
#include "growl/ui/pack.h"

using Growl::Align;
using Growl::Container;

void Container::layout() {
	int i = 0;
	for (auto& pack : pack_info) {
		auto node = getChildren().at(i++);
		auto widget = Growl::nodeAsWidget(node);

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

		node->setWidth(pack.resolvedWidth);
		node->setHeight(pack.resolvedHeight);

		float margin_top = pack.marginTop.evaluate(node);
		float margin_bottom = pack.marginBottom.evaluate(node);
		float margin_left = pack.marginLeft.evaluate(node);
		float margin_right = pack.marginRight.evaluate(node);

		switch (pack.align_along) {
		case Align::LEFT:
			node->setX(margin_left);
			break;
		case Align::MIDDLE:
			node->setX(
				(getWidth() - node->getWidth()) / 2 + margin_left -
				margin_right);
			break;
		case Align::RIGHT:
			node->setX((getWidth() - node->getWidth()) + margin_left);
			break;
		default:
			break;
		}

		switch (pack.align_across) {
		case Align::TOP:
			node->setY(margin_top);
			break;
		case Align::MIDDLE:
			node->setY(
				((getHeight() + margin_top - margin_bottom) -
				 node->getHeightRaw()) /
				2);
			break;
		case Align::BOTTOM:
			node->setY((getHeight() - node->getHeightRaw()) - margin_bottom);
			break;
		default:
			break;
		}

		if (widget) {
			widget->invalidate();
		}
	}
}
