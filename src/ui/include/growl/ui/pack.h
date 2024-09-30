#pragma once

namespace Growl {

class Node;

struct PackInfo {
	bool expand = false;
	bool fill = false;

	int prefWidth = 0;
	int prefHeight = 0;
};

class Packer {
public:
	Packer(Node* node, PackInfo* pack)
		: node{node}
		, pack{pack} {}

	Node* getNode() {
		return node;
	}

	Packer& expand();
	Packer& fill();
	Packer& width(int width);
	Packer& height(int height);

private:
	Node* node;
	PackInfo* pack;
};

} // namespace Growl
