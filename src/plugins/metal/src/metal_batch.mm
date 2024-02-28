#include "metal_batch.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/assets/font_face.h"
#include "growl/core/graphics/shader.h"
#include "metal_buffer.h"
#include "metal_graphics.h"
#include "metal_texture.h"
#include <cmath>
#include <memory>
#include <vector>

using Growl::MetalBatch;
using Growl::Shader;

void MetalBatch::clear(float r, float g, float b) {
	clear_color = MTLClearColorMake(r, g, b, 1);
	should_clear = true;
}

void MetalBatch::begin() {
	surface =
		target_texture ? target_texture : metal_graphics.getSurface().texture;

	encoder = [metal_graphics.getCommandBuffer()
		renderCommandEncoderWithDescriptor:renderPassDescriptor()];

	auto projection =
		glm::ortho<float>(0, surface.width, surface.height, 0, 1, -1);

	constant_buffer = std::make_unique<MetalBuffer>(
		metal_graphics.getCurrentConstantBuffer());
	vertex_buffer =
		std::make_unique<MetalBuffer>(metal_graphics.getCurrentVertexBuffer());

	constant_buffer->writeAndBind(encoder, 0, &projection, sizeof(projection));
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));
}

void MetalBatch::end() {
	[encoder endEncoding];
	encoder = nil;
	surface = nil;
}

void MetalBatch::setColor(float r, float g, float b, float a) {
	color = {r, g, b, a};
}

void MetalBatch::setTransform(glm::mat4x4 transform) {
	this->transform = transform;
	constant_buffer->writeAndBind(
		encoder, 2, &this->transform, sizeof(this->transform));
}

glm::mat4x4 MetalBatch::getTransform() {
	return transform;
}

void MetalBatch::draw(
	const Texture& texture, float x, float y, float width, float height) {
	auto& tex = static_cast<const MetalTexture&>(texture);
	tex.bind(encoder);
	default_shader->bind(surface, encoder);

	float right = std::round(x + width);
	float bottom = std::round(y + height);
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

	float right = std::round(x + width);
	float bottom = std::round(y + height);

	auto& reg = region.region;
	std::vector<float> vertices;
	addVertex(vertices, right, bottom, reg.u1, reg.v1);
	addVertex(vertices, x, bottom, reg.u0, reg.v1);
	addVertex(vertices, x, y, reg.u0, reg.v0);
	addVertex(vertices, right, bottom, reg.u1, reg.v1);
	addVertex(vertices, x, y, reg.u0, reg.v0);
	addVertex(vertices, right, y, reg.u1, reg.v0);
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
	if (font_texture_atlas.getType() == FontFaceType::MSDF) {
		sdf_shader->bind(surface, encoder);
	} else {
		default_shader->bind(surface, encoder);
	}

	std::vector<float> vertices;
	for (auto& glyph : glyph_layout.getLayout()) {
		float gx = std::round(x + glyph.x);
		float gy = std::round(y + glyph.y);
		float right = std::round(gx + glyph.w);
		float bottom = std::round(gy + glyph.h);

		// Replace this by translating the glyphs in the layout?
		const auto& region_result =
			font_texture_atlas.getRegion(glyph.glyph_id);
		if (region_result.hasError()) {
			continue;
		}
		auto& reg = region_result.get();

		addVertex(vertices, right, bottom, reg.u1, reg.v1);
		addVertex(vertices, gx, bottom, reg.u0, reg.v1);
		addVertex(vertices, gx, gy, reg.u0, reg.v0);
		addVertex(vertices, right, bottom, reg.u1, reg.v1);
		addVertex(vertices, gx, gy, reg.u0, reg.v0);
		addVertex(vertices, right, gy, reg.u1, reg.v0);
	}
	vertex_buffer->writeAndBind(
		encoder, 1, vertices.data(), vertices.size() * sizeof(float));
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:glyph_layout.getLayout().size() * 6];
}

void MetalBatch::drawRect(float x, float y, float width, float height) {
	drawRect(x, y, width, height, *rect_shader);
}

void MetalBatch::drawRect(
	float x, float y, float width, float height, Shader& shader) {
	static_cast<MetalShader&>(shader).bind(surface, encoder);
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
	return static_cast<int>(surface.width);
}

int MetalBatch::getTargetHeight() {
	return static_cast<int>(surface.height);
}

MTLRenderPassDescriptor* MetalBatch::renderPassDescriptor() {
	MTLRenderPassDescriptor* pass =
		[MTLRenderPassDescriptor renderPassDescriptor];
	pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
	pass.colorAttachments[0].storeAction = MTLStoreActionStore;
	pass.colorAttachments[0].texture = surface;
	if (should_clear) {
		pass.colorAttachments[0].loadAction = MTLLoadActionClear;
		pass.colorAttachments[0].clearColor = clear_color;
		should_clear = false;
	}
	return pass;
}

void MetalBatch::addVertex(
	std::vector<float>& vertices, float x, float y, float tex_x, float tex_y) {
	vertices.insert(
		vertices.end(),
		{x, y, tex_x, tex_y, color.r, color.g, color.b, color.a});
}
