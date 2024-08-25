#pragma once

#include "growl/core/graphics/batch.h"
#include "growl/core/graphics/color.h"
#include <Metal/Metal.h>

#include "metal_buffer.h"
#include "metal_shader.h"

namespace Growl {

class MetalGraphicsAPI;
class API;

class MetalBatch : public Batch {
public:
	MetalBatch(
		API& api, MetalGraphicsAPI& metal_graphics, MetalShader* default_shader,
		MetalShader* rect_shader, MetalShader* sdf_shader,
		id<MTLTexture> target_texture = nil);

	~MetalBatch() {
#ifdef GROWL_IMGUI
		if (im_surface) {
			[im_surface release];
		}
#endif
	}

	void clear(float r, float g, float b) override;
	void begin() override;
	void end() override;

	Color getColor() override {
		return color;
	}
	void setColor(float r, float g, float b, float a) override;

	void draw(
		const Texture& texture, float x, float y, float width, float height,
		glm::mat4x4 transform = glm::identity<glm::mat4x4>()) override;
	void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height, glm::mat4x4 transform) override;
	void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y,
		glm::mat4x4 transform) override;

	void drawRect(
		float x, float y, float width, float height,
		glm::mat4x4 transform) override;
	void drawRect(
		float x, float y, float width, float height, Shader& shader,
		glm::mat4x4 transform) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	API& api;
	MetalGraphicsAPI& metal_graphics;
	id<MTLRenderCommandEncoder> encoder;
	MetalShader* default_shader;
	MetalShader* rect_shader;
	MetalShader* sdf_shader;
	id<MTLTexture> target_texture;
	std::unique_ptr<MetalBuffer> constant_buffer;
	std::unique_ptr<MetalBuffer> vertex_buffer;
	Color color;
	bool should_clear = false;
	MTLClearColor clear_color;
	id<MTLTexture> surface;
#ifdef GROWL_IMGUI
	id<MTLTexture> im_surface;
	int im_w;
	int im_h;
#endif

	MTLRenderPassDescriptor* renderPassDescriptor();
	void addVertex(
		std::vector<float>& vertices, float x, float y, float tex_x,
		float tex_y);
};

} // namespace Growl
