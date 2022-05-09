#include "metal_batch.h"
#include "metal_texture.h"
#include <memory>
#include <vector>

using Growl::MetalBatch;

void MetalBatch::begin() {
	encoder = [command_buffer
		renderCommandEncoderWithDescriptor:renderPassDescriptor()];
	[encoder setVertexBuffer:constant_buffer offset:0 atIndex:0];
}

void MetalBatch::end() {
	[encoder endEncoding];
	[constant_buffer release];
}

void MetalBatch::setColor(float r, float g, float b, float a) {
	color = {r, g, b, a};
}

void MetalBatch::draw(
	const Texture& texture, float x, float y, float width, float height) {
	auto& tex = static_cast<const MetalTexture&>(texture);
	tex.bind(encoder);
	default_shader->bind(surface, encoder);

	float right = x + width;
	float bottom = y + height;
	std::vector<float> vertices;
	addVertex(vertices, right, bottom, 1.f, 1.f);
	addVertex(vertices, x, bottom, 0.f, 1.f);
	addVertex(vertices, x, y, 0.f, 0.f);
	addVertex(vertices, right, bottom, 1.f, 1.f);
	addVertex(vertices, x, y, 0.f, 0.f);
	addVertex(vertices, right, y, 1.f, 0.f);
	[encoder setVertexBytes:vertices.data()
					 length:vertices.size() * sizeof(float)
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
	default_shader->bind(surface, encoder);

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

	std::vector<float> vertices;
	addVertex(vertices, right, bottom, tex_right, tex_bottom);
	addVertex(vertices, x, bottom, tex_left, tex_bottom);
	addVertex(vertices, x, y, tex_left, tex_top);
	addVertex(vertices, right, bottom, tex_right, tex_bottom);
	addVertex(vertices, x, y, tex_left, tex_top);
	addVertex(vertices, right, y, tex_right, tex_top);
	[encoder setVertexBytes:vertices.data()
					 length:vertices.size() * sizeof(float)
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
	default_shader->bind(surface, encoder);

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

		float tex_left = (region.x + 0.5) / (float)tex.getWidth();
		float tex_top = (region.y + 0.5) / (float)tex.getHeight();
		float tex_right = (region.x + region.w + 0.5) / (float)tex.getWidth();
		float tex_bottom = (region.y + region.h + 0.5) / (float)tex.getHeight();

		addVertex(vertices, right, bottom, tex_right, tex_bottom);
		addVertex(vertices, gx, bottom, tex_left, tex_bottom);
		addVertex(vertices, gx, gy, tex_left, tex_top);
		addVertex(vertices, right, bottom, tex_right, tex_bottom);
		addVertex(vertices, gx, gy, tex_left, tex_top);
		addVertex(vertices, right, gy, tex_right, tex_top);
	}
	[encoder setVertexBytes:vertices.data()
					 length:vertices.size() * sizeof(float)
					atIndex:1];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:vertices.size() / 4];
}

void MetalBatch::drawRect(float x, float y, float width, float height) {
	rect_shader->bind(surface, encoder);
	float right = x + width;
	float bottom = y + height;
	std::vector<float> vertices;
	addVertex(vertices, right, bottom, 1.f, 1.f);
	addVertex(vertices, x, bottom, 0.f, 1.f);
	addVertex(vertices, x, y, 0.f, 0.f);
	addVertex(vertices, right, bottom, 1.f, 1.f);
	addVertex(vertices, x, y, 0.f, 0.f);
	addVertex(vertices, right, y, 1.f, 0.f);
	[encoder setVertexBytes:vertices.data()
					 length:vertices.size() * sizeof(float)
					atIndex:1];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:6];
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

void MetalBatch::addVertex(
	std::vector<float>& vertices, float x, float y, float tex_x, float tex_y) {
	vertices.insert(
		vertices.end(),
		{x, y, tex_x, tex_y, color.r, color.g, color.b, color.a});
}
