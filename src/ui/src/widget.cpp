#include "growl/ui/widget.h"
#include "growl/scene/node.h"
#include "growl/ui/pack.h"

using Growl::Node;
using Growl::Packer;
using Growl::Widget;

constexpr size_t WIDGET_SIGNIFIER = 0x2DFACE;

Widget::Widget(std::string&& label)
	: Node{std::move(label)}
	, pack_info{std::vector<PackInfo>()} {
	setUserData(WIDGET_SIGNIFIER);
}

Node* Widget::addChild(std::unique_ptr<Node> node) {
	pack_info.push_back(PackInfo{});
	return Node::addChild(std::move(node));
}

Packer Widget::addWithLayout(std::unique_ptr<Node> child) {
	Node* n = addChild(std::move(child));
	return Packer{n, &(pack_info.at(pack_info.size() - 1))};
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
