#include "opengl_batch.h"
#include "glm/gtc/type_ptr.hpp"
#include "growl/core/api/api.h"
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
#ifdef GROWL_IMGUI
#include "growl/core/imgui.h"
#include "imgui.h"
#endif

using Growl::API;
using Growl::OpenGLBatch;
using Growl::Shader;

OpenGLBatch::OpenGLBatch(
	API* api, OpenGLShader* default_shader, OpenGLShader* sdf_shader,
	OpenGLShader* rect_shader, int width, int height, Window* window,
	GLuint fbo)
	: api{api}
	, default_shader{default_shader}
	, sdf_shader{sdf_shader}
	, rect_shader{rect_shader}
	, width{width}
	, height{height}
	, color{1, 1, 1, 1}
	, window{window}
	, fbo{fbo} {
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glGenBuffers(1, &ubo_v);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_v);
	glBufferData(
		GL_UNIFORM_BUFFER, (MAX_BATCH_SIZE + 1) * sizeof(glm::mat4), nullptr,
		GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glGenBuffers(1, &ubo_f);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo_f);
	glBufferData(
		GL_UNIFORM_BUFFER, sizeof(FragmentBlock), nullptr, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

#ifdef GROWL_IMGUI
	if (!fbo) {
		im_w = width;
		im_h = height;
		glGenFramebuffers(1, &im_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, im_fbo);
		glGenTextures(1, &im_tex);
		glBindTexture(GL_TEXTURE_2D, im_tex);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB, im_w, im_h, 0, GL_RGB, GL_UNSIGNED_BYTE,
			NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, im_tex, 0);
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
#endif
}

OpenGLBatch::~OpenGLBatch() {
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	glDeleteBuffers(1, &ubo_v);
	glDeleteBuffers(1, &ubo_f);
	glDeleteVertexArrays(1, &vao);
	if (fbo) {
		glDeleteFramebuffers(1, &fbo);
	}
#ifdef GROWL_IMGUI
	if (im_fbo) {
		glDeleteFramebuffers(1, &im_fbo);
	}
	if (im_tex) {
		glDeleteTextures(1, &im_tex);
	}
#endif
}

void OpenGLBatch::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLBatch::begin() {
	if (!fbo && window) {
		window->getSize(&width, &height);
	}
	if (fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	}
#ifdef GROWL_IMGUI
	if (api->imguiVisible() && im_fbo) {
		glBindFramebuffer(GL_FRAMEBUFFER, im_fbo);
		imGuiBeginGameWindow();
		int new_w, new_h;
		imGuiGameWindowSize(&new_w, &new_h);
		if (new_w != im_w || new_h != im_h) {
			im_w = new_w;
			im_h = new_h;
			glBindTexture(GL_TEXTURE_2D, im_tex);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_RGB, im_w, im_h, 0, GL_RGB,
				GL_UNSIGNED_BYTE, NULL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(
				GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, im_tex, 0);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
#endif
	glViewport(0, 0, getTargetWidth(), getTargetHeight());
	auto projection = glm::ortho<float>(
		0, static_cast<float>(getTargetWidth()),
		fbo ? 0 : static_cast<float>(getTargetHeight()),
		fbo ? static_cast<float>(getTargetHeight()) : 0, 1, -1);

	glBindVertexArray(vao);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo_v, 0, sizeof(glm::mat4));
	glBufferSubData(
		GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));

	FragmentBlock fragment{
		glm::vec2{getTargetWidth(), getTargetHeight()},
		static_cast<float>(api->frameTimer().getTotalTime()),
		static_cast<float>(api->frameTimer().getDeltaTime())};
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, ubo_f);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(fragment), &fragment);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLBatch::end() {
	flush();
	glBindVertexArray(0);
#ifdef GROWL_IMGUI
	if (api->imguiVisible() && im_fbo) {
		ImGui::Image(
			reinterpret_cast<ImTextureID>(im_tex),
			ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
		imGuiEndGameWindow();
	}
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLBatch::setColor(float r, float g, float b, float a) {
	color = {r, g, b, a};
}

void OpenGLBatch::draw(
	const Texture& texture, float x, float y, float width, float height,
	glm::mat4x4 transform) {
	auto& tex = static_cast<const OpenGLTexture&>(texture);
	if (&tex != bound_tex || default_shader != bound_shader ||
		idx >= MAX_BATCH_SIZE) {
		flush();
	}
	uniforms.insert(uniforms.end(), VertexBlock{transform});
	bound_tex = &tex;
	bound_shader = default_shader;
	float right = x + width;
	float bottom = y + height;
	addVertex(x, y, 0.0f, 0.0f);
	addVertex(right, y, 1.0f, 0.0f);
	addVertex(right, bottom, 1.0f, 1.0f);
	addVertex(x, bottom, 0.0f, 1.0f);
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
	if (&tex != bound_tex || default_shader != bound_shader ||
		idx >= MAX_BATCH_SIZE) {
		flush();
	}
	uniforms.insert(uniforms.end(), VertexBlock{transform});
	bound_tex = &tex;
	bound_shader = default_shader;
	float right = x + width;
	float bottom = y + height;
	addVertex(x, y, region.region.u0, region.region.v0);
	addVertex(right, y, region.region.u1, region.region.v0);
	addVertex(right, bottom, region.region.u1, region.region.v1);
	addVertex(x, bottom, region.region.u0, region.region.v1);
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
	if (&tex != bound_tex || shader != bound_shader || idx >= MAX_BATCH_SIZE) {
		flush();
	}
	uniforms.insert(uniforms.end(), VertexBlock{transform});
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

		addVertex(gx, gy, region.u0, region.v0);
		addVertex(right, gy, region.u1, region.v0);
		addVertex(right, bottom, region.u1, region.v1);
		addVertex(gx, bottom, region.u0, region.v1);
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
	if (bound_tex || bound_shader != &gl_shader || idx >= MAX_BATCH_SIZE) {
		flush();
	}
	bound_shader = &gl_shader;

	uniforms.insert(uniforms.end(), VertexBlock{transform});

	float right = x + width;
	float bottom = y + height;
	addVertex(x, y, 0.0f, 0.0f);
	addVertex(right, y, 1.0f, 0.0f);
	addVertex(right, bottom, 1.0f, 0.0f);
	addVertex(x, bottom, 0.0f, 1.0f);
	elements.insert(
		elements.end(),
		{verts, verts + 1, verts + 2, verts + 2, verts + 3, verts});
	verts += 4;
	idx++;
}

int OpenGLBatch::getTargetWidth() {
#ifdef GROWL_IMGUI
	if (im_fbo && api->imguiVisible()) {
		return im_w;
	}
#endif
	return width;
}

int OpenGLBatch::getTargetHeight() {
#ifdef GROWL_IMGUI
	if (im_fbo && api->imguiVisible()) {
		return im_h;
	}
#endif
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
		glBindBuffer(GL_UNIFORM_BUFFER, ubo_v);
		glBufferSubData(
			GL_UNIFORM_BUFFER, sizeof(glm::mat4),
			sizeof(VertexBlock) * uniforms.size(), uniforms.data());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}

	if (bound_tex) {
		bound_tex->bind();
	}
	if (bound_shader) {
		bound_shader->bind();
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

void OpenGLBatch::addVertex(float x, float y, float tex_x, float tex_y) {
	vertices.insert(
		vertices.end(), {x, y, tex_x, tex_y, color.r, color.g, color.b, color.a,
						 static_cast<GLfloat>(idx)});
}
