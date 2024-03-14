#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/graphics/batch.h"
#include "growl/core/graphics/color.h"
#include "opengl.h"

namespace Growl {

class Window;
class OpenGLShader;
class OpenGLTexture;
class FontTextureAtlas;
class GlyphLayout;
class Texture;
struct TextureAtlasRegion;

struct SpriteBlock {
	glm::mat4x4 transform;
};

class OpenGLBatch : public Batch {
public:
	OpenGLBatch(
		OpenGLShader* default_shader, OpenGLShader* sdf_shader,
		OpenGLShader* rect_shader, int width, int height, Window* window,
		GLuint fbo);
	~OpenGLBatch();

	void clear(float r, float g, float b) override;
	void begin() override;
	void end() override;

	void setColor(float r, float g, float b, float a) override;

	void draw(
		const Texture& texture, float x, float y, float width, float height,
		glm::mat4x4 transform) override;
	void draw(
		const TextureAtlasRegion& texture, float x, float y, float width,
		float height, glm::mat4x4 transform) override;
	void draw(
		const GlyphLayout& glyph_layout,
		const FontTextureAtlas& font_texture_atlas, float x, float y,
		glm::mat4x4 transform) override;

	void drawRect(
		float x, float y, float width, float height,
		glm::mat4x4 transform) override;
	void drawRect(
		float x, float y, float width, float height, Shader& shader,
		glm::mat4x4 transform) override;

	int getTargetWidth() override;
	int getTargetHeight() override;

private:
	OpenGLShader* default_shader;
	OpenGLShader* sdf_shader;
	OpenGLShader* rect_shader;
	int width;
	int height;
	Color color;
	Window* window;
	GLuint vao = 0;
	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint fbo = 0;
	GLuint ubo = 0;

	unsigned int idx = 0;
	unsigned int verts = 0;
	const OpenGLTexture* bound_tex = nullptr;
	OpenGLShader* bound_shader = nullptr;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> elements;
	std::vector<SpriteBlock> uniforms;

	void flush();
	void addVertex(float x, float y, float tex_x, float tex_y);
};

} // namespace Growl
