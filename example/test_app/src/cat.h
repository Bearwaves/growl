#pragma once

#include "growl/scene/node.h"
namespace Growl {

class Batch;
class TextureAtlas;

class Cat : public Node {
public:
	explicit Cat(std::string&& label, TextureAtlas* atlas)
		: Node{std::move(label)}
		, atlas{atlas} {}
	void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;
	void onMouseEvent(InputMouseEvent& event) override;
	void onTouchEvent(InputTouchEvent& event) override;

private:
	TextureAtlas* atlas;
	bool is_hit = false;
};

} // namespace Growl
