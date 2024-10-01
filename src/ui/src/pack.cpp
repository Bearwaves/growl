#include "growl/ui/pack.h"

using Growl::Node;
using Growl::Packer;

Packer& Packer::expand() {
	pack->expand = true;
	return *this;
}

Packer& Packer::fill() {
	pack->fill = true;
	return *this;
}

Packer& Packer::width(int width) {
	pack->prefWidth = width;
	return *this;
}

Packer& Packer::height(int height) {
	pack->prefHeight = height;
	return *this;
}

Packer& Packer::align(Align align) {
	pack->alignment = align;
	return *this;
}
