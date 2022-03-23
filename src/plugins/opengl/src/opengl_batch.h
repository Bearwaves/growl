#pragma once

#include "opengl_graphics.h"
#include "opengl_shader.h"
#include <growl/core/graphics/batch.h>

namespace Growl {

class OpenGLBatch : public Batch {
public:
	explicit OpenGLBatch(OpenGLShader* shader, glm::mat4 mvp)
		: shader{shader}
		, mvp{mvp} {}
	~OpenGLBatch();
	void begin() override;
	void end() override;
	void draw(
		const Texture& texture, float x, float y, float width,
		float height) override;

private:
	OpenGLShader* shader;
	glm::mat4 mvp;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

} // namespace Growl
