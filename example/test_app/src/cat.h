#pragma once

#include "growl/scene/node.h"
namespace Growl {

class Batch;
class TextureAtlas;

class Cat : public Node {
public:
	explicit Cat(TextureAtlas* atlas)
		: atlas{atlas} {}
	void onDraw(Batch& batch, float parent_alpha) override;
	void onMouseEvent(InputMouseEvent& event) override;
	void onTouchEvent(InputTouchEvent& event) override;

private:
	TextureAtlas* atlas;
	bool is_hit = false;
};

} // namespace Growl
