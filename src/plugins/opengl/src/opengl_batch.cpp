#include "opengl_batch.h"
#include "opengl.h"
#include "opengl_texture.h"

using Growl::OpenGLBatch;

OpenGLBatch::~OpenGLBatch() {
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

void OpenGLBatch::begin() {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
}

void OpenGLBatch::end() {
	glBindVertexArray(0);
}

void OpenGLBatch::draw(
	const Texture& texture, float x, float y, float width, float height) {
	auto& tex = static_cast<const OpenGLTexture&>(texture);
	tex.bind();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	float right = x + width;
	float bottom = y + height;
	float quadVertexData[] = {
		x,	   y,	   0.0f, 0.0f, // Top-left
		right, y,	   1.0f, 0.0f, // Top-right
		right, bottom, 1.0f, 1.0f, // Bottom-right
		x,	   bottom, 0.0f, 1.0f  // Bottom-left
	};
	GLuint elements[] = {0, 1, 2, 2, 3, 0};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData,
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
	float texLeft = (region.region.x + 0.5) / (float)tex.getWidth();
	float texTop = (region.region.y + 0.5) / (float)tex.getHeight();
	float texRight =
		(region.region.x + region.region.width + 0.5) / (float)tex.getWidth();
	float texBottom =
		(region.region.y + region.region.height + 0.5) / (float)tex.getHeight();
	float quadVertexData[] = {
		x,	   y,	   texLeft,	 texTop,	right, y,	   texRight, texTop,
		right, bottom, texRight, texBottom, x,	   bottom, texLeft,	 texBottom};
	GLuint elements[] = {0, 1, 2, 2, 3, 0};
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER, sizeof(quadVertexData), quadVertexData,
		GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
	shader->bind(mvp);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

int OpenGLBatch::getTargetWidth() {
	int w, h;
	SDL_GetWindowSize(static_cast<SDL_Window*>(window->getNative()), &w, &h);
	return w;
}

int OpenGLBatch::getTargetHeight() {
	int w, h;
	SDL_GetWindowSize(static_cast<SDL_Window*>(window->getNative()), &w, &h);
	return h;
}
