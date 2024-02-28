#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "growl/core/graphics/batch.h"
#include "growl/core/graphics/color.h"
#include <Metal/Metal.h>

#include "metal_buffer.h"
#include "metal_shader.h"

namespace Growl {

class MetalGraphicsAPI;

class MetalBatch : public Batch {
public:
	MetalBatch(
		MetalGraphicsAPI& metal_graphics, MetalShader* default_shader,
		MetalShader* rect_shader, MetalShader* sdf_shader,
		id<MTLTexture> target_texture = nil)
		: metal_graphics{metal_graphics}
		, default_shader{default_shader}
		, rect_shader{rect_shader}
		, sdf_shader{sdf_shader}
		, target_texture{target_texture}
		, constant_buffer{nil}
		, vertex_buffer{nil}
		, color{1, 1, 1, 1}
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
	void drawRect(
		float x, float y, float width, float height, Shader& shader) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	MetalGraphicsAPI& metal_graphics;
	id<MTLRenderCommandEncoder> encoder;
	MetalShader* default_shader;
	MetalShader* rect_shader;
	MetalShader* sdf_shader;
	id<MTLTexture> target_texture;
	std::unique_ptr<MetalBuffer> constant_buffer;
	std::unique_ptr<MetalBuffer> vertex_buffer;
	Color color;
	glm::mat4x4 transform;
	bool should_clear = false;
	MTLClearColor clear_color;
	id<MTLTexture> surface;

	MTLRenderPassDescriptor* renderPassDescriptor();
	void addVertex(
		std::vector<float>& vertices, float x, float y, float tex_x,
		float tex_y);
};

} // namespace Growl
