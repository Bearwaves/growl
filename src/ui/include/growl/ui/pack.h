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

template <class T>
class Packer {
public:
	Packer(T* node, PackInfo* pack)
		: node{node}
		, pack{pack} {}

	T* getNode() {
		return node;
	}

	Packer<T>& expand() {
		pack->expand = true;
		return *this;
	}

	Packer<T>& fill() {
		pack->fill = true;
		return *this;
	}

	Packer<T>& width(Value width) {
		pack->prefWidth = width;
		return *this;
	}

	Packer<T>& height(Value height) {
		pack->prefHeight = height;
		return *this;
	}

	Packer<T>& align(Align align) {
		pack->alignment = align;
		return *this;
	}

	Packer<T>& margin(Value margin) {
		pack->marginTop = margin;
		pack->marginBottom = margin;
		pack->marginLeft = margin;
		pack->marginRight = margin;
		return *this;
	}

	Packer<T>& marginTop(Value margin) {
		pack->marginTop = margin;
		return *this;
	}

	Packer<T>& marginBottom(Value margin) {
		pack->marginBottom = margin;
		return *this;
	}

	Packer<T>& marginLeft(Value margin) {
		pack->marginLeft = margin;
		return *this;
	}

	Packer<T>& marginRight(Value margin) {
		pack->marginRight = margin;
		return *this;
	}

	Packer<T>& marginX(Value margin) {
		pack->marginLeft = margin;
		pack->marginRight = margin;
		return *this;
	}

	Packer<T>& marginY(Value margin) {
		pack->marginTop = margin;
		pack->marginBottom = margin;
		return *this;
	}

private:
	T* node;
	PackInfo* pack;
};

} // namespace Growl
