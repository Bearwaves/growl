#pragma once

#include "growl/core/graphics/texture_atlas.h"
#include "growl/scene/node.h"

namespace Growl {

struct TextureAtlasRegion;
class Texture;

class Sprite : public Node {
public:
	Sprite(std::string&& label, TextureAtlasRegion&& region)
		: Node{std::move(label)}
		, region{std::make_unique<TextureAtlasRegion>(std::move(region))}
		, texture{nullptr} {}
	Sprite(std::string&& label, Texture* texture)
		: Node{std::move(label)}
		, region{nullptr}
		, texture{texture} {}

	void
	onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) override;

	void setRegion(TextureAtlasRegion&& region);
	void setTexture(Texture* texture);

private:
	std::unique_ptr<TextureAtlasRegion> region;
	Texture* texture;
};

} // namespace Growl
