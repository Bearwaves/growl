#pragma once

#include "font_texture_atlas.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/graphics/shader.h"
#include "growl/core/text/glyph_layout.h"
#include "texture.h"
#include "texture_atlas.h"

namespace Growl {

class Batch {
public:
	virtual ~Batch() = default;
	virtual void clear(float r, float g, float b) = 0;
	virtual void begin() = 0;
	virtual void end() = 0;

	virtual void setColor(float r, float g, float b, float a) = 0;
	virtual void setTransform(glm::mat4x4 transform) = 0;
	virtual glm::mat4x4 getTransform() = 0;

	virtual void draw(
		const Texture& texture, float x, float y, float width,
		float height) = 0;
	virtual void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height) = 0;
	virtual void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y) = 0;

	virtual void drawRect(float x, float y, float width, float height) = 0;
	virtual void
	drawRect(float x, float y, float width, float height, Shader& shader) = 0;

	virtual int getTargetWidth() = 0;
	virtual int getTargetHeight() = 0;
};

} // namespace Growl
