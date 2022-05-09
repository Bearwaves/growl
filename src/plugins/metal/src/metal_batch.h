#pragma once

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
		id<MTLBuffer> constant_buffer)
		: command_buffer{command_buffer}
		, surface{surface}
		, default_shader{default_shader}
		, rect_shader{rect_shader}
		, constant_buffer{constant_buffer}
		, color{1, 1, 1, 1} {}
	void begin() override;
	void end() override;
	void setColor(float r, float g, float b, float a) override;

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
	id<MTLBuffer> constant_buffer;
	Color color;

	MTLRenderPassDescriptor* renderPassDescriptor();
	void addVertex(
		std::vector<float>& vertices, float x, float y, float tex_x,
		float tex_y);
};

} // namespace Growl
