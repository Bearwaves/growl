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
	float quadVertexData[] = {right, bottom, 1.f, 1.f, x,	  bottom, 0.f, 1.f,
							  x,	 y,		 0.f, 0.f, right, bottom, 1.f, 1.f,
							  x,	 y,		 0.f, 0.f, right, y,	  1.f, 0.f};
	[encoder setVertexBytes:quadVertexData
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
	float texLeft = (region.region.x + 0.5) / (float)tex.getWidth();
	float texTop = (region.region.y + 0.5) / (float)tex.getHeight();
	float texRight =
		(region.region.x + region.region.width + 0.5) / (float)tex.getWidth();
	float texBottom =
		(region.region.y + region.region.height + 0.5) / (float)tex.getHeight();

	float quadVertexData[] = {
		right, bottom, texRight, texBottom, x,	   bottom, texLeft,	 texBottom,
		x,	   y,	   texLeft,	 texTop,	right, bottom, texRight, texBottom,
		x,	   y,	   texLeft,	 texTop,	right, y,	   texRight, texTop};
	[encoder setVertexBytes:quadVertexData
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
		const auto& regionResult = font_texture_atlas.getRegion(glyph.glyph_id);
		if (regionResult.hasError()) {
			continue;
		}
		auto region = regionResult.get();

		float texLeft = region.x / (float)tex.getWidth();
		float texTop = region.y / (float)tex.getHeight();
		float texRight = (region.x + region.w) / (float)tex.getWidth();
		float texBottom = (region.y + region.h) / (float)tex.getHeight();
		vertices.insert(vertices.end(), {right, bottom, texRight, texBottom,
										 gx,	bottom, texLeft,  texBottom,
										 gx,	gy,		texLeft,  texTop,
										 right, bottom, texRight, texBottom,
										 gx,	gy,		texLeft,  texTop,
										 right, gy,		texRight, texTop});
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
