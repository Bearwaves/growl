#include "opengl_batch.h"
#include "opengl.h"
#include "opengl_texture.h"
#include <vector>

using Growl::OpenGLBatch;

OpenGLBatch::~OpenGLBatch() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
	if (fbo) {
		glDeleteFramebuffers(1, &fbo);
	}
}

void OpenGLBatch::begin() {
	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
}

void OpenGLBatch::end() {
	glBindVertexArray(0);
	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void OpenGLBatch::draw(
	const Texture& texture, float x, float y, float width, float height) {
	auto& tex = static_cast<const OpenGLTexture&>(texture);
	tex.bind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	float right = x + width;
	float bottom = y + height;
	float quad_vertex_data[] = {
		x,	   y,	   0.0f, 0.0f, // Top-left
		right, y,	   1.0f, 0.0f, // Top-right
		right, bottom, 1.0f, 1.0f, // Bottom-right
		x,	   bottom, 0.0f, 1.0f  // Bottom-left
	};
	GLuint elements[] = {0, 1, 2, 2, 3, 0};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	shader->bind(mvp);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLBatch::draw(
	const TextureAtlasRegion& region, float x, float y, float width,
	float height) {
	auto& tex = static_cast<const OpenGLTexture&>(region.atlas->getTexture());
	tex.bind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	float right = x + width;
	float bottom = y + height;
	// Address texel centres
	float tex_left = (region.region.x + 0.5f) / (float)tex.getWidth();
	float tex_top = (region.region.y + 0.5f) / (float)tex.getHeight();
	float tex_right =
		(region.region.x + region.region.width + 0.5f) / (float)tex.getWidth();
	float tex_bottom = (region.region.y + region.region.height + 0.5f) /
					   (float)tex.getHeight();
	float quad_vertex_data[] = {x,	   y,	   tex_left,  tex_top,
								right, y,	   tex_right, tex_top,
								right, bottom, tex_right, tex_bottom,
								x,	   bottom, tex_left,  tex_bottom};
	GLuint elements[] = {0, 1, 2, 2, 3, 0};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	shader->bind(mvp);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void OpenGLBatch::draw(
	const GlyphLayout& glyph_layout, const FontTextureAtlas& font_texture_atlas,
	float x, float y) {
	auto& tex =
		static_cast<const OpenGLTexture&>(font_texture_atlas.getTexture());
	tex.bind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	std::vector<float> vertices;
	std::vector<GLuint> indices;
	GLuint i = 0;
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
		auto& region = region_result.get();

		float tex_left = region.x / (float)tex.getWidth();
		float tex_top = region.y / (float)tex.getHeight();
		float tex_right = (region.x + region.w) / (float)tex.getWidth();
		float tex_bottom = (region.y + region.h) / (float)tex.getHeight();
		vertices.insert(
			vertices.end(),
			{gx, gy, tex_left, tex_top, right, gy, tex_right, tex_top, right,
			 bottom, tex_right, tex_bottom, gx, bottom, tex_left, tex_bottom});
		indices.insert(indices.end(), {i, i + 1, i + 2, i + 2, i + 3, i});
		i += 4;
	}

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(),
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
		indices.data(), GL_STATIC_DRAW);
	shader->bind(mvp);
	glDrawElements(
		GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}

int OpenGLBatch::getTargetWidth() {
	return width;
}

int OpenGLBatch::getTargetHeight() {
	return height;
}
