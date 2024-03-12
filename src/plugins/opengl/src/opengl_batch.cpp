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
		GL_UNIFORM_BUFFER, (MAX_BATCH_SIZE + 1) * sizeof(glm::mat4), NULL,
		GL_DYNAMIC_DRAW);
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
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, sizeof(glm::mat4));
	glBufferSubData(
		GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLBatch::end() {
	flush();
	glBindVertexArray(0);
	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void OpenGLBatch::setColor(float r, float g, float b, float a) {
	color = {r, g, b, a};
}

void OpenGLBatch::draw(
	const Texture& texture, float x, float y, float width, float height,
	glm::mat4x4 transform) {
	auto& tex = static_cast<const OpenGLTexture&>(texture);
	if (&tex != bound_tex || default_shader != bound_shader) {
		flush();
	}
	uniforms.insert(uniforms.end(), SpriteBlock{transform});
	bound_tex = &tex;
	bound_shader = default_shader;
	float right = x + width;
	float bottom = y + height;
	vertices.insert(
		vertices.end(), {
							x,
							y,
							0.0f,
							0.0f,
							static_cast<GLfloat>(idx), // Top-left
							right,
							y,
							1.0f,
							0.0f,
							static_cast<GLfloat>(idx), // Top-right
							right,
							bottom,
							1.0f,
							1.0f,
							static_cast<GLfloat>(idx), // Bottom-right
							x,
							bottom,
							0.0f,
							1.0f,
							static_cast<GLfloat>(idx) // Bottom-left
						});
	elements.insert(
		elements.end(),
		{verts, verts + 1, verts + 2, verts + 2, verts + 3, verts});
	idx++;
	verts += 4;
}

void OpenGLBatch::draw(
	const TextureAtlasRegion& region, float x, float y, float width,
	float height, glm::mat4x4 transform) {
	auto& tex = static_cast<const OpenGLTexture&>(region.atlas->getTexture());
	if (&tex != bound_tex || default_shader != bound_shader) {
		flush();
	}
	uniforms.insert(uniforms.end(), SpriteBlock{transform});
	bound_tex = &tex;
	bound_shader = default_shader;
	float right = x + width;
	float bottom = y + height;
	vertices.insert(
		vertices.end(), {
							x,
							y,
							region.region.u0,
							region.region.v0,
							static_cast<GLfloat>(idx),
							right,
							y,
							region.region.u1,
							region.region.v0,
							static_cast<GLfloat>(idx),
							right,
							bottom,
							region.region.u1,
							region.region.v1,
							static_cast<GLfloat>(idx),
							x,
							bottom,
							region.region.u0,
							region.region.v1,
							static_cast<GLfloat>(idx),
						});
	elements.insert(
		elements.end(),
		{verts, verts + 1, verts + 2, verts + 2, verts + 3, verts});
	idx++;
	verts += 4;
}

void OpenGLBatch::draw(
	const GlyphLayout& glyph_layout, const FontTextureAtlas& font_texture_atlas,
	float x, float y, glm::mat4x4 transform) {
	auto& tex =
		static_cast<const OpenGLTexture&>(font_texture_atlas.getTexture());
	auto shader = font_texture_atlas.getType() == FontFaceType::MSDF
					  ? sdf_shader
					  : default_shader;
	if (&tex != bound_tex || shader != bound_shader) {
		flush();
	}
	uniforms.insert(uniforms.end(), SpriteBlock{transform});
	bound_tex = &tex;
	bound_shader = shader;

	GLuint i = verts;
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
			vertices.end(),
			{gx,	gy,		region.u0, region.v0, static_cast<GLfloat>(idx),
			 right, gy,		region.u1, region.v0, static_cast<GLfloat>(idx),
			 right, bottom, region.u1, region.v1, static_cast<GLfloat>(idx),
			 gx,	bottom, region.u0, region.v1, static_cast<GLfloat>(idx)});
		elements.insert(elements.end(), {i, i + 1, i + 2, i + 2, i + 3, i});
		i += 4;
		verts = i;
	}
	idx++;
}

void OpenGLBatch::drawRect(
	float x, float y, float width, float height, glm::mat4x4 transform) {
	drawRect(x, y, width, height, *rect_shader, transform);
}

void OpenGLBatch::drawRect(
	float x, float y, float width, float height, Shader& shader,
	glm::mat4x4 transform) {

	auto& gl_shader = static_cast<OpenGLShader&>(shader);
	uniforms.insert(uniforms.end(), SpriteBlock{transform});

	if (bound_tex || bound_shader != &gl_shader) {
		flush();
	}
	bound_shader = &gl_shader;

	float right = x + width;
	float bottom = y + height;
	vertices.insert(
		vertices.end(), {
							x,
							y,
							0.0f,
							0.0f,
							static_cast<GLfloat>(idx), // Top-left
							right,
							y,
							1.0f,
							0.0f,
							static_cast<GLfloat>(idx), // Top-right
							right,
							bottom,
							1.0f,
							1.0f,
							static_cast<GLfloat>(idx), // Bottom-right
							x,
							bottom,
							0.0f,
							1.0f,
							static_cast<GLfloat>(idx) // Bottom-left
						});
	elements.insert(
		elements.end(),
		{verts, verts + 1, verts + 2, verts + 2, verts + 3, verts});
	verts += 4;
	idx++;
}

int OpenGLBatch::getTargetWidth() {
	return width;
}

int OpenGLBatch::getTargetHeight() {
	return height;
}

void OpenGLBatch::flush() {
	if (!idx) {
		return;
	}

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(),
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, elements.size() * sizeof(GLuint),
		elements.data(), GL_DYNAMIC_DRAW);

	if (uniforms.size()) {
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(
			GL_UNIFORM_BUFFER, sizeof(glm::mat4),
			sizeof(SpriteBlock) * uniforms.size(), uniforms.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	if (bound_tex) {
		bound_tex->bind();
	}
	if (bound_shader) {
		bound_shader->bind(color);
	}

	glDrawElements(GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0);

	idx = 0;
	verts = 0;
	vertices.clear();
	elements.clear();
	uniforms.clear();
	bound_tex = nullptr;
	bound_shader = nullptr;
}
