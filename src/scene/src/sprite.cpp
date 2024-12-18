#include "growl/scene/sprite.h"
#include "growl/core/graphics/batch.h"

using Growl::Batch;
using Growl::Sprite;

void Sprite::onDraw(Batch& batch, float parent_alpha, glm::mat4x4 transform) {
	if (region) {
		batch.draw(*region, 0, 0, getWidth(), getHeight(), transform);
	} else if (texture) {
		batch.draw(*texture, 0, 0, getWidth(), getHeight(), transform);
	}
}

void Sprite::setRegion(TextureAtlasRegion&& region) {
	this->region = std::make_unique<TextureAtlasRegion>(std::move(region));
}

void Sprite::setTexture(Texture* texture) {
	this->texture = texture;
}
