#pragma once

#include "growl/scene/node.h"

namespace Growl {

class Widget : public Node {
public:
	Widget(std::string&& name);

	void invalidate();
	void validate();

protected:
	virtual void layout() = 0;
	virtual void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

	bool invalidated = true;
};

bool nodeIsWidget(Node& node);

} // namespace Growl
