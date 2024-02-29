#include "opengl_batch.h"
#include "glm/gtc/type_ptr.hpp"
#include "growl/core/assets/atlas.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/graphics/font_texture_atlas.h"
#include "growl/core/graphics/shader.h"
#include "growl/core/graphics/texture_atlas.h"
#include "growl/core/graphics/window.h"
#include "opengl_shader.h"
#include "opengl_texture.h"
#include <cmath>
#include <vector>

using Growl::OpenGLBatch;
using Growl::Shader;

OpenGLBatch::OpenGLBatch(
	OpenGLShader* default_shader, OpenGLShader* sdf_shader,
	OpenGLShader* rect_shader, int width, int height, Window* window,
	GLuint fbo)
	: default_shader{default_shader}
	, sdf_shader{sdf_shader}
	, rect_shader{rect_shader}
	, transform{glm::identity<glm::mat4x4>()}
	, width{width}
	, height{height}
	, color{1, 1, 1, 1}
	, window{window}
	, fbo{fbo} {
	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(
		GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

OpenGLBatch::~OpenGLBatch() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &ubo);
	glDeleteVertexArrays(1, &vao);
	if (fbo) {
		glDeleteFramebuffers(1, &fbo);
	}
}

void OpenGLBatch::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLBatch::begin() {
	if (!fbo && window) {
		window->getSize(&width, &height);
	}
	glViewport(0, 0, width, height);
	auto projection = glm::ortho<float>(
		0, static_cast<float>(width), fbo ? 0 : static_cast<float>(height),
		fbo ? static_cast<float>(height) : 0, 1, -1);

	glBindVertexArray(vao);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 2 * sizeof(glm::mat4));
	glBufferSubData(
		GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBufferSubData(
		GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
		glm::value_ptr(transform));
}

void OpenGLBatch::end() {
	glBindVertexArray(0);
	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void OpenGLBatch::setColor(float r, float g, float b, float a) {
	color = {r, g, b, a};
}

void OpenGLBatch::setTransform(glm::mat4x4 transform) {
	this->transform = transform;
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferSubData(
		GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4),
		glm::value_ptr(transform));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

glm::mat4x4 OpenGLBatch::getTransform() {
	return transform;
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
	default_shader->bind(color);
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
	float quad_vertex_data[] = {
		x,	   y,	   region.region.u0, region.region.v0,
		right, y,	   region.region.u1, region.region.v0,
		right, bottom, region.region.u1, region.region.v1,
		x,	   bottom, region.region.u0, region.region.v1};
	GLuint elements[] = {0, 1, 2, 2, 3, 0};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(quad_vertex_data), quad_vertex_data,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	default_shader->bind(color);
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
		auto& region = region_result.get();

		vertices.insert(
			vertices.end(), {gx, gy, region.u0, region.v0, right, gy, region.u1,
							 region.v0, right, bottom, region.u1, region.v1, gx,
							 bottom, region.u0, region.v1});
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
	if (font_texture_atlas.getType() == FontFaceType::MSDF) {
		sdf_shader->bind(color);
	} else {
		default_shader->bind(color);
	}
	glDrawElements(
		GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
}

void OpenGLBatch::drawRect(float x, float y, float width, float height) {
	drawRect(x, y, width, height, *rect_shader);
}

void OpenGLBatch::drawRect(
	float x, float y, float width, float height, Shader& shader) {
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
	static_cast<OpenGLShader&>(shader).bind(color);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int OpenGLBatch::getTargetWidth() {
	return width;
}

int OpenGLBatch::getTargetHeight() {
	return height;
}
