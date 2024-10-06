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
