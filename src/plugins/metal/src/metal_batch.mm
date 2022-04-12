#include "metal_batch.h"
#include "metal_texture.h"
#include <memory>

using Growl::MetalBatch;

void MetalBatch::begin() {
	encoder = [command_buffer
		renderCommandEncoderWithDescriptor:renderPassDescriptor()];
	shader->bind(surface, encoder);
	[encoder setVertexBuffer:constant_buffer offset:0 atIndex:0];
}

void MetalBatch::end() {
	[encoder endEncoding];
	[constant_buffer release];
}

void MetalBatch::draw(
	const Texture& texture, float x, float y, float width, float height) {
	auto& tex = static_cast<const MetalTexture&>(texture);
	tex.bind(encoder);

	float right = x + width;
	float bottom = y + height;
	float quad_vertex_data[] = {
		right, bottom, 1.f, 1.f, x, bottom, 0.f, 1.f, x,	 y, 0.f, 0.f,
		right, bottom, 1.f, 1.f, x, y,		0.f, 0.f, right, y, 1.f, 0.f};
	[encoder setVertexBytes:quad_vertex_data
					 length:6 * 4 * sizeof(float)
					atIndex:1];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:6];
}

void MetalBatch::draw(
	const TextureAtlasRegion& region, float x, float y, float width,
	float height) {
	auto& tex = static_cast<const MetalTexture&>(region.atlas->getTexture());
	tex.bind(encoder);

	float right = x + width;
	float bottom = y + height;

	// Address texel centres
	// TODO don't?
	float tex_left = (region.region.x + 0.5) / (float)tex.getWidth();
	float tex_top = (region.region.y + 0.5) / (float)tex.getHeight();
	float tex_right =
		(region.region.x + region.region.width + 0.5) / (float)tex.getWidth();
	float tex_bottom =
		(region.region.y + region.region.height + 0.5) / (float)tex.getHeight();

	float quad_vertex_data[] = {
		right,	  bottom,	  tex_right, tex_bottom, x,			bottom,
		tex_left, tex_bottom, x,		 y,			 tex_left,	tex_top,
		right,	  bottom,	  tex_right, tex_bottom, x,			y,
		tex_left, tex_top,	  right,	 y,			 tex_right, tex_top};
	[encoder setVertexBytes:quad_vertex_data
					 length:6 * 4 * sizeof(float)
					atIndex:1];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:6];
}

void MetalBatch::draw(
	const GlyphLayout& glyph_layout, const FontTextureAtlas& font_texture_atlas,
	float x, float y) {
	auto& tex =
		static_cast<const MetalTexture&>(font_texture_atlas.getTexture());
	tex.bind(encoder);

	std::vector<float> vertices;
	for (auto& glyph : glyph_layout.getLayout()) {
		float gx = x + glyph.x;
		float gy = y + glyph.y;
		float right = gx + glyph.w;
		float bottom = gy + glyph.h;

		// Replace this by translating the glyphs in the layout?
		const auto& region_result =
			font_texture_atlas.getRegion(glyph.glyph_id);
		if (region_result.hasError()) {
			continue;
		}
		auto region = region_result.get();

		float tex_left = region.x / (float)tex.getWidth();
		float tex_top = region.y / (float)tex.getHeight();
		float tex_right = (region.x + region.w) / (float)tex.getWidth();
		float tex_bottom = (region.y + region.h) / (float)tex.getHeight();
		vertices.insert(vertices.end(), {right, bottom, tex_right, tex_bottom,
										 gx,	bottom, tex_left,  tex_bottom,
										 gx,	gy,		tex_left,  tex_top,
										 right, bottom, tex_right, tex_bottom,
										 gx,	gy,		tex_left,  tex_top,
										 right, gy,		tex_right, tex_top});
	}
	[encoder setVertexBytes:vertices.data()
					 length:vertices.size() * sizeof(float)
					atIndex:1];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:vertices.size() / 4];
}

int MetalBatch::getTargetWidth() {
	return surface.width;
}

int MetalBatch::getTargetHeight() {
	return surface.height;
}

MTLRenderPassDescriptor* MetalBatch::renderPassDescriptor() {
	MTLRenderPassDescriptor* pass =
		[MTLRenderPassDescriptor renderPassDescriptor];
	pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
	pass.colorAttachments[0].storeAction = MTLStoreActionStore;
	pass.colorAttachments[0].texture = surface;
	return pass;
}
