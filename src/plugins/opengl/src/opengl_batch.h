#pragma once

#include "glm/ext/matrix_float4x4.hpp"
#include "growl/core/graphics/batch.h"
#include "growl/core/graphics/color.h"
#include "opengl.h"

namespace Growl {

class Window;
class OpenGLShader;
class OpenGLTexture;
class OpenGLGraphicsAPI;
class FontTextureAtlas;
class GlyphLayout;
class Texture;
class API;
struct TextureAtlasRegion;

struct VertexBlock {
	glm::mat4x4 transform;
};

struct FragmentBlock {
	glm::vec2 resolution;
	float time;
	float deltaTime;
};

class OpenGLBatch : public Batch {
public:
	OpenGLBatch(
		API* api, OpenGLShader* default_shader, OpenGLShader* sdf_shader,
		OpenGLShader* rect_shader, int width, int height, Window* window,
		GLuint fbo);
	~OpenGLBatch();

	void clear(float r, float g, float b) override;
	void begin() override;
	void end() override;

	Color getColor() override {
		return color;
	}
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
	API* api;
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
	GLuint ubo_v = 0;
	GLuint ubo_f = 0;

	unsigned int idx = 0;
	unsigned int verts = 0;
	const OpenGLTexture* bound_tex = nullptr;
	OpenGLShader* bound_shader = nullptr;
	std::vector<GLfloat> vertices;
	std::vector<GLuint> elements;
	std::vector<VertexBlock> uniforms;

#ifdef GROWL_IMGUI
	GLuint im_fbo = 0;
	GLuint im_tex = 0;
	int im_w;
	int im_h;
#endif

	void flush();
	void addVertex(float x, float y, float tex_x, float tex_y);
};

} // namespace Growl
