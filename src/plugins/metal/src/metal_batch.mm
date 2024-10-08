#include "metal_batch.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/assets/font_face.h"
#include "growl/core/graphics/shader.h"
#ifdef GROWL_IMGUI
#include "growl/core/imgui.h"
#include "imgui.h"
#endif
#include "metal_buffer.h"
#include "metal_graphics.h"
#include "metal_texture.h"
#include <cmath>
#include <memory>
#include <vector>

using Growl::MetalBatch;
using Growl::Shader;

struct ConstantBlock {
	glm::mat4x4 transform;
	glm::vec2 resolution;
	float time;
	float deltaTime;
};

MetalBatch::MetalBatch(
	API& api, MetalGraphicsAPI& metal_graphics, MetalShader* default_shader,
	MetalShader* rect_shader, MetalShader* sdf_shader,
	id<MTLTexture> target_texture)
	: api{api}
	, metal_graphics{metal_graphics}
	, default_shader{default_shader}
	, rect_shader{rect_shader}
	, sdf_shader{sdf_shader}
	, target_texture{target_texture}
	, constant_buffer{nil}
	, vertex_buffer{nil}
	, color{1, 1, 1, 1} {
#ifdef GROWL_IMGUI
	if (!target_texture) {
		im_surface = metal_graphics.createMetalTargetTexture(1, 1);
	}
#endif
}

void MetalBatch::clear(float r, float g, float b) {
	clear_color = MTLClearColorMake(r, g, b, 1);
	should_clear = true;
}

void MetalBatch::begin() {
	surface =
		target_texture ? target_texture : metal_graphics.getSurface().texture;
#ifdef GROWL_IMGUI
	if (api.imguiVisible() && im_surface) {
		imGuiBeginGameWindow();
		imGuiGameWindowSize(&im_w, &im_h);
		if (surface.width > im_surface.width ||
			surface.height > im_surface.height) {
			[im_surface release];
			im_surface = metal_graphics.createMetalTargetTexture(
				surface.width, surface.height);
		}
		surface = im_surface;
	}
#endif

	encoder = [metal_graphics.getCommandBuffer()
		renderCommandEncoderWithDescriptor:renderPassDescriptor()];

	[encoder setViewport:MTLViewport{
							 0, 0, static_cast<double>(getTargetWidth()),
							 static_cast<double>(getTargetHeight()), 0, 1}];

	auto projection =
		glm::ortho<float>(0, getTargetWidth(), getTargetHeight(), 0, 1, -1);

	constant_buffer = std::make_unique<MetalBuffer>(
		metal_graphics.getCurrentConstantBuffer());
	vertex_buffer =
		std::make_unique<MetalBuffer>(metal_graphics.getCurrentVertexBuffer());

	ConstantBlock block{
		projection, glm::vec2{getTargetWidth(), getTargetHeight()},
		static_cast<float>(api.frameTimer().getTotalTime()),
		static_cast<float>(api.frameTimer().getDeltaTime())};

	constant_buffer->writeAndBind(
		encoder, 0, &block, sizeof(block), BufferBinding::Both);
}

void MetalBatch::end() {
	[encoder endEncoding];
#ifdef GROWL_IMGUI
	if (api.imguiVisible() && im_surface) {
		ImGui::Image(
			reinterpret_cast<ImTextureID>(im_surface),
			ImGui::GetContentRegionAvail(), ImVec2(0, 0),
			ImVec2(
				static_cast<float>(im_w) / surface.width,
				static_cast<float>(im_h) / surface.height));
		imGuiEndGameWindow();
	}
#endif
	encoder = nil;
	surface = nil;
}

void MetalBatch::setColor(float r, float g, float b, float a) {
	color = {r, g, b, a};
}

void MetalBatch::draw(
	const Texture& texture, float x, float y, float width, float height,
	glm::mat4x4 transform) {
	auto& tex = static_cast<const MetalTexture&>(texture);
	tex.bind(encoder);
	default_shader->bind(surface, encoder);
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));

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
	float height, glm::mat4x4 transform) {
	auto& tex = static_cast<const MetalTexture&>(region.atlas->getTexture());
	tex.bind(encoder);
	default_shader->bind(surface, encoder);
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));

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
	float x, float y, glm::mat4x4 transform) {
	auto& tex =
		static_cast<const MetalTexture&>(font_texture_atlas.getTexture());
	tex.bind(encoder);
	if (font_texture_atlas.getType() == FontFaceType::MSDF) {
		sdf_shader->bind(surface, encoder);
	} else {
		default_shader->bind(surface, encoder);
	}
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));

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

void MetalBatch::drawRect(
	float x, float y, float width, float height, glm::mat4x4 transform,
	float border_width) {
	drawRect(
		x, y, width, height, *rect_shader, transform, border_width, nullptr, 0);
}

struct RectUniforms {
	glm::vec2 rect_size;
	float border_size;
};

void MetalBatch::drawRect(
	float x, float y, float width, float height, Shader& shader,
	glm::mat4x4 transform, float border_width, void* uniforms,
	int uniforms_length) {
	static_cast<MetalShader&>(shader).bind(surface, encoder);
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));

	if (!uniforms) {
		RectUniforms rect;
		rect.rect_size.x = width;
		rect.rect_size.y = height;
		rect.border_size = border_width;
		uniforms = &rect;
		uniforms_length = sizeof(rect);
	}
	constant_buffer->writeAndBind(
		encoder, 2, uniforms, uniforms_length, BufferBinding::Fragment);

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
#ifdef GROWL_IMGUI
	if (api.imguiVisible() && im_surface) {
		return im_w;
	}
#endif
	return static_cast<int>(surface.width);
}

int MetalBatch::getTargetHeight() {
#ifdef GROWL_IMGUI
	if (api.imguiVisible() && im_surface) {
		return im_h;
	}
#endif
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
