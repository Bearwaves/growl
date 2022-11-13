#pragma once

#include "growl/scene/node.h"
namespace Growl {

class Batch;
class TextureAtlas;

class Cat : public Node {
public:
	explicit Cat(TextureAtlas* atlas)
		: atlas{atlas} {}
	void draw(Batch& batch, float parent_alpha);

private:
	TextureAtlas* atlas;
};

} // namespace Growl
