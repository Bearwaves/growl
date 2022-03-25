#pragma once

#include "opengl_graphics.h"
#include "opengl_shader.h"
#include <growl/core/graphics/batch.h>

namespace Growl {

class OpenGLBatch : public Batch {
public:
	explicit OpenGLBatch(OpenGLShader* shader, glm::mat4 mvp, Window* window)
		: shader{shader}
		, mvp{mvp}
		, window{window} {}
	~OpenGLBatch();
	void begin() override;
	void end() override;
	void draw(
		const Texture& texture, float x, float y, float width,
		float height) override;
	void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	OpenGLShader* shader;
	glm::mat4 mvp;
	Window* window;
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
};

} // namespace Growl
