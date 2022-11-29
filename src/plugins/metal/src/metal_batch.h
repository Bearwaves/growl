#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "growl/core/graphics/batch.h"
#include "growl/core/graphics/color.h"
#include <Metal/Metal.h>

#include "metal_shader.h"

namespace Growl {

class MetalBatch : public Batch {
public:
	MetalBatch(
		id<MTLCommandBuffer> command_buffer, id<MTLTexture> surface,
		MetalShader* default_shader, MetalShader* rect_shader,
		MetalShader* sdf_shader, glm::mat4x4 projection,
		id<MTLBuffer> constant_buffer, uint32_t* constant_offset,
		id<MTLBuffer> vertex_buffer, uint32_t* vertex_offset)
		: command_buffer{command_buffer}
		, surface{surface}
		, default_shader{default_shader}
		, rect_shader{rect_shader}
		, sdf_shader{sdf_shader}
		, constant_buffer{constant_buffer}
		, constant_offset{constant_offset}
		, vertex_buffer{vertex_buffer}
		, vertex_offset{vertex_offset}
		, color{1, 1, 1, 1}
		, projection{projection}
		, transform{glm::identity<glm::mat4x4>()} {}

	void clear(float r, float g, float b) override;
	void begin() override;
	void end() override;
	void setColor(float r, float g, float b, float a) override;

	void setTransform(glm::mat4x4 transform) override;
	glm::mat4x4 getTransform() override;

	void draw(
		const Texture& texture, float x, float y, float width,
		float height) override;
	void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height) override;
	void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y) override;

	void drawRect(float x, float y, float width, float height) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	id<MTLCommandBuffer> command_buffer;
	id<MTLTexture> surface;
	id<MTLRenderCommandEncoder> encoder;
	MetalShader* default_shader;
	MetalShader* rect_shader;
	MetalShader* sdf_shader;
	id<MTLBuffer> constant_buffer;
	uint32_t* constant_offset;
	id<MTLBuffer> vertex_buffer;
	uint32_t* vertex_offset;
	Color color;
	glm::mat4x4 projection;
	glm::mat4x4 transform;
	bool should_clear = false;
	MTLClearColor clear_color;

	MTLRenderPassDescriptor* renderPassDescriptor();
	void addVertex(
		std::vector<float>& vertices, float x, float y, float tex_x,
		float tex_y);
};

} // namespace Growl
