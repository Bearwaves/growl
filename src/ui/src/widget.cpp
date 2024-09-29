#include "growl/ui/widget.h"
#include "growl/scene/node.h"

using Growl::Widget;

constexpr size_t WIDGET_SIGNIFIER = 0x2DFACE;

Widget::Widget(std::string&& label)
	: Node(std::move(label)) {
	setUserData(WIDGET_SIGNIFIER);
}

void Widget::invalidate() {
	invalidated = true;
}

void Widget::validate() {
	if (invalidated) {
		layout();
		invalidated = false;
	}
}

void Widget::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	validate();
	Node::onDraw(batch, parent_alpha, transform);
}

bool Growl::nodeIsWidget(Node& node) {
	return node.getUserData() == WIDGET_SIGNIFIER;
}
