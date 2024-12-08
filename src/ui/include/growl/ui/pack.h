#pragma once

namespace Growl {

class Node;

enum class Align { LEFT, TOP, MIDDLE, RIGHT, BOTTOM };

class Value {
public:
	Value()
		: Value{0, Type::NONE, nullptr} {}
	Value(float v)
		: Value{v, Type::FIXED, nullptr} {}
	float evaluate(Node* context);
	operator bool() {
		return type != Type::NONE;
	}

	static Value percentWidth(float percent, Node* of = nullptr) {
		return Value{percent, Type::PERCENT_WIDTH, of};
	}

	static Value percentHeight(float percent, Node* of = nullptr) {
		return Value{percent, Type::PERCENT_HEIGHT, of};
	}

private:
	enum class Type { NONE, FIXED, PERCENT_WIDTH, PERCENT_HEIGHT };

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
	bool fill_along = false;
	bool fill_across = false;

	Value prefWidth;
	Value prefHeight;
	Value maxWidth;
	Value maxHeight;

	Value marginLeft;
	Value marginRight;
	Value marginTop;
	Value marginBottom;

	Align align_along = Align::MIDDLE;
	Align align_across = Align::MIDDLE;

	float boundsWidth = 0;
	float boundsHeight = 0;
	float prefWidthResult = 0;
	float prefHeightResult = 0;
	float resolvedWidth = 0;
	float resolvedHeight = 0;
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
		pack->fill_along = true;
		pack->fill_across = true;
		return *this;
	}

	Packer<T>& fillAlong() {
		pack->fill_along = true;
		return *this;
	}

	Packer<T>& fillAcross() {
		pack->fill_across = true;
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

	Packer<T>& maxWidth(Value max_width) {
		pack->maxWidth = max_width;
		return *this;
	}

	Packer<T>& maxHeight(Value max_height) {
		pack->maxHeight = max_height;
		return *this;
	}

	Packer<T>& alignAlong(Align align) {
		pack->align_along = align;
		return *this;
	}

	Packer<T>& alignAcross(Align align) {
		pack->align_across = align;
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
