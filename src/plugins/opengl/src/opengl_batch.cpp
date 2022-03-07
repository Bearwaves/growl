#include "opengl_batch.h"
#include "opengl_texture.h"
#include <GLES3/gl3.h>

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
	Texture* texture, float x, float y, float width, float height) {
	auto tex = static_cast<OpenGLTexture*>(texture);
	tex->bind();
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