#include "growl/ui/pack.h"
#include "growl/scene/node.h"

using Growl::Node;
using Growl::Packer;
using Growl::Value;

float Value::evaluate(Node* context) {
	switch (type) {
	case Type::FIXED:
		return value;
	case Type::PERCENT_WIDTH:
		return value * (this->of ? of->getWidth() : context->getWidth());
	case Type::PERCENT_HEIGHT:
		return value * (this->of ? of->getHeight() : context->getHeight());
	}
	return 0;
}

Packer& Packer::expand() {
	pack->expand = true;
	return *this;
}

Packer& Packer::fill() {
	pack->fill = true;
	return *this;
}

Packer& Packer::width(Value width) {
	pack->prefWidth = width;
	return *this;
}

Packer& Packer::height(Value height) {
	pack->prefHeight = height;
	return *this;
}

Packer& Packer::align(Align align) {
	pack->alignment = align;
	return *this;
}
