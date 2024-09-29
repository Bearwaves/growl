#pragma once

#include "growl/core/graphics/color.h"
#include "growl/core/graphics/texture_atlas.h"
#include "growl/scene/node.h"

namespace Growl {

struct TextureAtlasRegion;
class Texture;

class Rectangle : public Node {
public:
	Rectangle(std::string&& label, Color color)
		: Node{std::move(label)}
		, color(std::move(color)) {}

	void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

private:
	std::unique_ptr<TextureAtlasRegion> region;
	Color color;
};

} // namespace Growl
