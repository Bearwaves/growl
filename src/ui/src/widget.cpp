#include "growl/ui/widget.h"
#include "growl/scene/node.h"
#include "growl/ui/pack.h"

using Growl::Node;
using Growl::Packer;
using Growl::Widget;

constexpr size_t WIDGET_SIGNIFIER = 0x2DFACE;
constexpr int VALIDATION_MAX_PASSES = 5;

Widget::Widget(std::string&& label)
	: Node{std::move(label)}
	, pack_info{std::vector<PackInfo>()} {
	setUserData(WIDGET_SIGNIFIER);
}

Node* Widget::addChild(std::unique_ptr<Node> node) {
	pack_info.push_back(PackInfo{});
	return Node::addChild(std::move(node));
}

Node* Widget::addChild(Node* node) {
	pack_info.push_back(PackInfo{});
	return Node::addChild(node);
}

void Widget::clear() {
	Node::clear();
	pack_info.clear();
	invalidate();
}

void Widget::invalidate() {
	invalidated = true;
}

void Widget::invalidateHierarchy() {
	invalidate();
	if (getParent()) {
		if (auto widget = nodeAsWidget(getParent())) {
			widget->invalidateHierarchy();
		}
	}
}

void Widget::validate() {
	if (!invalidated) {
		return;
	}

	invalidated = false;
	layout();

	if (invalidated) {
		if (getParent() && nodeAsWidget(getParent())) {
			// We can rely on the root to call validate again.
			return;
		}
	}
	for (int i = 0; i < VALIDATION_MAX_PASSES; i++) {
		invalidated = false;
		layout();
		if (!invalidated) {
			break;
		}
	}
}

void Widget::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	validate();
	Node::onDraw(batch, parent_alpha, transform);
}

Widget* Growl::nodeAsWidget(Node* node) {
	if (node->getUserData() == WIDGET_SIGNIFIER) {
		return static_cast<Widget*>(node);
	}
	return nullptr;
}

void Widget::removeChild(int i) {
	Node::removeChild(i);
	pack_info.erase(std::next(pack_info.begin(), i));
}
