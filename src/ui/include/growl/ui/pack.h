#pragma once

namespace Growl {

class Node;

enum class Align { START, MIDDLE, END };

class Value {
public:
	Value(float v)
		: Value{v, Type::FIXED, nullptr} {}
	float evaluate(Node* context);

	static Value percentWidth(float percent, Node* of = nullptr) {
		return Value{percent, Type::PERCENT_WIDTH, of};
	}

	static Value percentHeight(float percent, Node* of = nullptr) {
		return Value{percent, Type::PERCENT_HEIGHT, of};
	}

private:
	enum class Type { FIXED, PERCENT_WIDTH, PERCENT_HEIGHT };

	Value(float value, Type type, Node* of)
		: value{value}
		, type{type}
		, of{of} {}

	float value;
	Type type;
	Node* of;
};

struct PackInfo {
	bool expand = false;
	bool fill = false;

	Value prefWidth = 0;
	Value prefHeight = 0;

	Value marginLeft = 0;
	Value marginRight = 0;
	Value marginTop = 0;
	Value marginBottom = 0;

	Align alignment = Align::START;

	float computedWidth = 0;
	float computedHeight = 0;
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
	Packer& width(Value width);
	Packer& height(Value height);
	Packer& align(Align align);

	Packer& margin(Value margin);
	Packer& marginTop(Value margin);
	Packer& marginBottom(Value margin);
	Packer& marginLeft(Value margin);
	Packer& marginRight(Value margin);
	Packer& marginX(Value margin);
	Packer& marginY(Value margin);

private:
	Node* node;
	PackInfo* pack;
};

} // namespace Growl
