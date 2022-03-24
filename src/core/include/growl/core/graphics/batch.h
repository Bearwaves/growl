#pragma once

#include "texture.h"
#include "texture_atlas.h"

namespace Growl {

class Batch {
public:
	virtual ~Batch() = default;
	virtual void begin() = 0;
	virtual void end() = 0;
	virtual void draw(
		const Texture& texture, float x, float y, float width,
		float height) = 0;
	virtual void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height) = 0;
};

} // namespace Growl
