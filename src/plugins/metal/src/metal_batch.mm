#include "metal_batch.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/assets/font_face.h"
#include "growl/core/graphics/shader.h"
#include <Metal/Metal.h>
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

struct MetalBatch::Vertex {
	float x, y;
	float tex_x, tex_y;
	float r, g, b, a;
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

void MetalBatch::setScissor(
	float x, float y, float w, float h, glm::mat4x4 transform) {
	auto world_coordinates = transform * glm::vec4{x, y, 0, 1};
	[encoder setScissorRect:MTLScissorRect{
								static_cast<NSUInteger>(world_coordinates.x),
								static_cast<NSUInteger>(world_coordinates.y),
								static_cast<NSUInteger>(w),
								static_cast<NSUInteger>(h),
							}];
}

void MetalBatch::resetScissor() {
	[encoder setScissorRect:MTLScissorRect{
								0,
								0,
								static_cast<NSUInteger>(getTargetWidth()),
								static_cast<NSUInteger>(getTargetHeight()),
							}];
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
	Vertex vertices[6];
	setVertex(vertices[0], right, bottom, 1.f, 1.f);
	setVertex(vertices[1], x, bottom, 0.f, 1.f);
	setVertex(vertices[2], x, y, 0.f, 0.f);
	setVertex(vertices[3], right, bottom, 1.f, 1.f);
	setVertex(vertices[4], x, y, 0.f, 0.f);
	setVertex(vertices[5], right, y, 1.f, 0.f);
	[encoder setVertexBytes:vertices length:6 * sizeof(Vertex) atIndex:1];
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
	Vertex vertices[6];
	setVertex(vertices[0], right, bottom, reg.u1, reg.v1);
	setVertex(vertices[1], x, bottom, reg.u0, reg.v1);
	setVertex(vertices[2], x, y, reg.u0, reg.v0);
	setVertex(vertices[3], right, bottom, reg.u1, reg.v1);
	setVertex(vertices[4], x, y, reg.u0, reg.v0);
	setVertex(vertices[5], right, y, reg.u1, reg.v0);
	[encoder setVertexBytes:vertices length:6 * sizeof(Vertex) atIndex:1];
	[encoder drawPrimitives:MTLPrimitiveTypeTriangle
				vertexStart:0
				vertexCount:6];
}

struct SDFUniforms {
	glm::vec2 texture_size;
	float pixel_range;
	float _padding;
};

void MetalBatch::draw(
	const GlyphLayout& glyph_layout, const FontTextureAtlas& font_texture_atlas,
	float x, float y, glm::mat4x4 transform) {
	auto& tex =
		static_cast<const MetalTexture&>(font_texture_atlas.getTexture());
	tex.bind(encoder);
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));
	if (font_texture_atlas.getType() == FontFaceType::MSDF) {
		sdf_shader->bind(surface, encoder);
		SDFUniforms uniforms{
			glm::vec2{
				font_texture_atlas.getTexture().getWidth(),
				font_texture_atlas.getTexture().getHeight()},
			font_texture_atlas.getPixelRange()};
		constant_buffer->writeAndBind(
			encoder, 2, &uniforms, sizeof(uniforms), BufferBinding::Fragment);
	} else {
		default_shader->bind(surface, encoder);
	}

	std::vector<Vertex> vertices(6 * glyph_layout.getLayout().size());
	size_t i = 0;
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

		setVertex(vertices[i++], right, bottom, reg.u1, reg.v1);
		setVertex(vertices[i++], gx, bottom, reg.u0, reg.v1);
		setVertex(vertices[i++], gx, gy, reg.u0, reg.v0);
		setVertex(vertices[i++], right, bottom, reg.u1, reg.v1);
		setVertex(vertices[i++], gx, gy, reg.u0, reg.v0);
		setVertex(vertices[i++], right, gy, reg.u1, reg.v0);
	}
	vertex_buffer->writeAndBind(
		encoder, 1, vertices.data(), i * sizeof(Vertex));
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

void MetalBatch::drawRect(
	float x, float y, float width, float height, Shader& shader,
	glm::mat4x4 transform, float border_width, void* uniforms,
	int uniforms_length) {
	static_cast<MetalShader&>(shader).bind(surface, encoder);
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));

	if (uniforms) {
		constant_buffer->writeAndBind(
			encoder, 2, uniforms, uniforms_length, BufferBinding::Fragment);
	}

	x = std::round(x) + 0.5f;
	y = std::round(y) + 0.5f;
	float right = x + width;
	float bottom = y + height;
	if (border_width == 0) {
		Vertex vertices[6];
		setVertex(vertices[0], right, bottom, 1.f, 1.f);
		setVertex(vertices[1], x, bottom, 0.f, 1.f);
		setVertex(vertices[2], x, y, 0.f, 0.f);
		setVertex(vertices[3], right, bottom, 1.f, 1.f);
		setVertex(vertices[4], x, y, 0.f, 0.f);
		setVertex(vertices[5], right, y, 1.f, 0.f);
		[encoder setVertexBytes:vertices length:6 * sizeof(Vertex) atIndex:1];
		[encoder drawPrimitives:MTLPrimitiveTypeTriangle
					vertexStart:0
					vertexCount:6];
	} else {
		border_width = std::max(1.0f, std::round(border_width));
		Vertex vertices[24];
		size_t i = 0;
		// Top
		float top_inner = y + border_width;
		setVertex(vertices[i++], right, top_inner, 1.f, 1.f);
		setVertex(vertices[i++], x, top_inner, 0.f, 1.f);
		setVertex(vertices[i++], x, y, 0.f, 0.f);
		setVertex(vertices[i++], right, top_inner, 1.f, 1.f);
		setVertex(vertices[i++], x, y, 0.f, 0.f);
		setVertex(vertices[i++], right, y, 1.f, 0.f);

		// Bottom
		float bottom_inner = y + height - border_width;
		setVertex(vertices[i++], right, bottom, 1.f, 1.f);
		setVertex(vertices[i++], x, bottom, 0.f, 1.f);
		setVertex(vertices[i++], x, bottom_inner, 0.f, 0.f);
		setVertex(vertices[i++], right, bottom, 1.f, 1.f);
		setVertex(vertices[i++], x, bottom_inner, 0.f, 0.f);
		setVertex(vertices[i++], right, bottom_inner, 1.f, 0.f);

		// Left
		float left_inner = x + border_width;
		setVertex(vertices[i++], left_inner, bottom, 1.f, 1.f);
		setVertex(vertices[i++], x, bottom, 0.f, 1.f);
		setVertex(vertices[i++], x, y, 0.f, 0.f);
		setVertex(vertices[i++], left_inner, bottom, 1.f, 1.f);
		setVertex(vertices[i++], x, y, 0.f, 0.f);
		setVertex(vertices[i++], left_inner, y, 1.f, 0.f);

		// Right
		float right_inner = x + width - border_width;
		setVertex(vertices[i++], right, bottom, 1.f, 1.f);
		setVertex(vertices[i++], right_inner, bottom, 0.f, 1.f);
		setVertex(vertices[i++], right_inner, y, 0.f, 0.f);
		setVertex(vertices[i++], right, bottom, 1.f, 1.f);
		setVertex(vertices[i++], right_inner, y, 0.f, 0.f);
		setVertex(vertices[i++], right, y, 1.f, 0.f);

		[encoder setVertexBytes:vertices length:24 * sizeof(Vertex) atIndex:1];
		[encoder drawPrimitives:MTLPrimitiveTypeTriangle
					vertexStart:0
					vertexCount:6 * 4];
	}
}

void MetalBatch::drawRect(
	float x, float y, float width, float height, Color gradient_top_left,
	Color gradient_top_right, Color gradient_bottom_left,
	Color gradient_bottom_right, glm::mat4x4 transform) {

	rect_shader->bind(surface, encoder);
	constant_buffer->writeAndBind(encoder, 2, &transform, sizeof(transform));

	Vertex vertices[6];
	x = std::round(x) + 0.5f;
	y = std::round(y) + 0.5f;
	float right = x + width;
	float bottom = y + height;
	setVertex(vertices[0], right, bottom, 1.f, 1.f, gradient_bottom_right);
	setVertex(vertices[1], x, bottom, 0.f, 1.f, gradient_bottom_left);
	setVertex(vertices[2], x, y, 0.f, 0.f, gradient_top_left);
	setVertex(vertices[3], right, bottom, 1.f, 1.f, gradient_bottom_right);
	setVertex(vertices[4], x, y, 0.f, 0.f, gradient_top_left);
	setVertex(vertices[5], right, y, 1.f, 0.f, gradient_top_right);
	[encoder setVertexBytes:vertices length:6 * sizeof(Vertex) atIndex:1];
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

void MetalBatch::setVertex(
	Vertex& v, float x, float y, float tex_x, float tex_y) {
	setVertex(v, x, y, tex_x, tex_y, this->color);
}

void MetalBatch::setVertex(
	Vertex& v, float x, float y, float tex_x, float tex_y, Color color) {
	v.x = x;
	v.y = y;
	v.tex_x = tex_x;
	v.tex_y = tex_y;
	v.r = color.r;
	v.g = color.g;
	v.b = color.b;
	v.a = color.a;
}
