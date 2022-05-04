#pragma once

#include "growl/core/graphics/batch.h"
#include "opengl_graphics.h"
#include "opengl_shader.h"

namespace Growl {

class OpenGLBatch : public Batch {
public:
	OpenGLBatch(
		OpenGLShader* default_shader, OpenGLShader* sdf_shader, glm::mat4 mvp,
		int width, int height, GLuint fbo)
		: default_shader{default_shader}
		, sdf_shader{sdf_shader}
		, mvp{mvp}
		, width{width}
		, height{height}
		, fbo{fbo} {}
	~OpenGLBatch();

	void begin() override;
	void end() override;

	void draw(
		const Texture& texture, float x, float y, float width,
		float height) override;
	void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height) override;
	void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	OpenGLShader* default_shader;
	OpenGLShader* sdf_shader;
	glm::mat4 mvp;
	int width;
	int height;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint fbo;
};

} // namespace Growl
