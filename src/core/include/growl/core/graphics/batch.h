#pragma once

#include "font_texture_atlas.h"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "growl/core/graphics/color.h"
#include "growl/core/graphics/shader.h"
#include "growl/core/text/glyph_layout.h"
#include "texture.h"
#include "texture_atlas.h"

namespace Growl {

class Batch {
public:
	Batch() = default;
	virtual ~Batch() = default;

	// Batch is move-only
	Batch(const Batch&) = delete;
	Batch& operator=(const Batch&) = delete;
	Batch(Batch&&) = default;
	Batch& operator=(Batch&&) = default;

	virtual void clear(float r, float g, float b) = 0;
	virtual void clear(Color color) {
		clear(color.r, color.g, color.b);
	}
	virtual void begin() = 0;
	virtual void end() = 0;

	virtual Color getColor() = 0;
	virtual void setColor(float r, float g, float b, float a) = 0;
	virtual void setColor(Color color) {
		setColor(color.r, color.g, color.b, color.a);
	}

	virtual void setScissor(
		float x, float y, float w, float h,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>()) = 0;
	virtual void resetScissor() = 0;

	virtual void draw(
		const Texture& texture, float x, float y, float width, float height,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>()) = 0;
	virtual void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height, glm::mat4x4 transform = glm::identity<glm::mat4x4>()) = 0;
	virtual void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>()) = 0;

	virtual void drawRect(
		float x, float y, float width, float height,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>(),
		float border_width = 0) = 0;
	virtual void drawRect(
		float x, float y, float width, float height, Shader& shader,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>(),
		float border_width = 0, void* uniforms = nullptr,
		int uniforms_length = 0) = 0;
	virtual void drawRect(
		float x, float y, float width, float height, Color gradient_top_left,
		Color gradient_top_right, Color gradient_bottom_left,
		Color gradient_bottom_right,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>()) = 0;

	virtual int getTargetWidth() = 0;
	virtual int getTargetHeight() = 0;
};

} // namespace Growl
