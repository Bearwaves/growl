#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/graphics/batch.h"
#include "growl/core/graphics/color.h"
#include "opengl.h"

namespace Growl {

class OpenGLShader;
class FontTextureAtlas;
class GlyphLayout;
class Texture;
struct TextureAtlasRegion;

class OpenGLBatch : public Batch {
public:
	OpenGLBatch(
		OpenGLShader* default_shader, OpenGLShader* sdf_shader,
		OpenGLShader* rect_shader, glm::mat4 mvp, int width, int height,
		GLuint fbo)
		: default_shader{default_shader}
		, sdf_shader{sdf_shader}
		, rect_shader{rect_shader}
		, mvp{mvp}
		, width{width}
		, height{height}
		, color{1, 1, 1, 1}
		, fbo{fbo} {}
	~OpenGLBatch();

	void clear(float r, float g, float b) override;
	void begin() override;
	void end() override;

	void setColor(float r, float g, float b, float a) override;
	void setTransform(glm::mat4x4 transform) override;
	glm::mat4x4 getTransform() override;

	void draw(
		const Texture& texture, float x, float y, float width,
		float height) override;
	void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height) override;
	void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y) override;

	void drawRect(float x, float y, float width, float height) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	OpenGLShader* default_shader;
	OpenGLShader* sdf_shader;
	OpenGLShader* rect_shader;
	glm::mat4 mvp;
	int width;
	int height;
	Color color;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint fbo;
};

} // namespace Growl
