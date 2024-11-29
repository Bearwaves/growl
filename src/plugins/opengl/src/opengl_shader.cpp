#include "opengl_shader.h"
#include "growl/core/error.h"
#include "opengl.h"
#include "opengl_error.h"
#include <vector>

using Growl::Error;
using Growl::OpenGLError;
using Growl::OpenGLShader;

OpenGLShader::~OpenGLShader() {
	if (program) {
		glDeleteProgram(program);
	}
}

constexpr GLsizei VERTEX_ATTRIB_STRIDE = 8 * sizeof(GLfloat) + sizeof(GLuint);

void OpenGLShader::bind() {
	glUseProgram(program);
	GLint pos_attrib = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(
		pos_attrib, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIB_STRIDE, 0);
	GLint tex_attrib = glGetAttribLocation(program, "texCoord");
	if (tex_attrib >= 0) {
		glEnableVertexAttribArray(tex_attrib);
		glVertexAttribPointer(
			tex_attrib, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIB_STRIDE,
			(void*)(2 * sizeof(GLfloat)));
	}
	GLuint color_attrib = glGetAttribLocation(program, "color");
	if (color_attrib >= 0) {
		glEnableVertexAttribArray(color_attrib);
		glVertexAttribPointer(
			color_attrib, 4, GL_FLOAT, GL_FALSE, VERTEX_ATTRIB_STRIDE,
			(void*)(4 * sizeof(GLfloat)));
	}
	GLuint index_attrib = glGetAttribLocation(program, "idx");
	if (index_attrib >= 0) {
		glEnableVertexAttribArray(index_attrib);
		glVertexAttribIPointer(
			index_attrib, 1, GL_UNSIGNED_INT, VERTEX_ATTRIB_STRIDE,
			(void*)(8 * sizeof(GLfloat)));
	}
}

Error OpenGLShader::compile() {
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	auto vertex_source = header + vertex_block + vertex_src;
	const char* vertex_source_c = vertex_source.c_str();
	glShaderSource(vertex, 1, &vertex_source_c, nullptr);
	glCompileShader(vertex);
	if (auto err = checkShaderCompileError(vertex)) {
		return err;
	}

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	auto fragment_source =
		header + uniforms_src + fragment_block + fragment_src;
	const char* fragment_source_c = fragment_source.c_str();
	glShaderSource(fragment, 1, &fragment_source_c, nullptr);
	glCompileShader(fragment);
	if (auto err = checkShaderCompileError(fragment)) {
		return err;
	}

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	GLuint vertex_block_index = glGetUniformBlockIndex(program, "VertexBlock");
	glUniformBlockBinding(program, vertex_block_index, 0);
	GLuint fragment_block_index =
		glGetUniformBlockIndex(program, "FragmentBlock");
	glUniformBlockBinding(program, fragment_block_index, 1);

	return nullptr;
}

Error OpenGLShader::checkShaderCompileError(unsigned int shader) {
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int info_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
		std::vector<char> log(info_len);
		glGetShaderInfoLog(shader, info_len, &info_len, log.data());
		return std::make_unique<OpenGLError>(
			std::string("Failed to compile shader: ") +
			std::string(log.data(), log.size()));
	}
	return nullptr;
}

std::string const OpenGLShader::header =
#ifdef GROWL_OPENGL_ES
	"#version 300 es\nprecision highp float;";
#else
	"#version 150 core\n";
#endif

const std::string OpenGLShader::default_uniforms = R"(
	struct Uniforms {
		float _padding;
	};
)";

const std::string OpenGLShader::vertex_block = R"(
in vec2 position;
in vec2 texCoord;
in vec4 color;
in int idx;

out vec2 TexCoord;
out vec4 Color;
flat out int Idx;

layout (std140) uniform VertexBlock {
	mat4 projection;
	mat4 transforms[100];
};
)";

const std::string OpenGLShader::fragment_block = R"(
layout (std140) uniform FragmentBlock {
	vec2 resolution;
	float time;
	float deltaTime;
	Uniforms uniforms[100];
};
)";

const std::string OpenGLShader::default_vertex = R"(
void main() {
	TexCoord = texCoord;
	Color = color;
	Idx = idx;
	gl_Position = projection * transforms[idx] * vec4(position, 0, 1);
}
)";

const std::string OpenGLShader::default_fragment = R"(
in vec2 TexCoord;
in vec4 Color;
out vec4 outCol;

uniform sampler2D texture0;

void main() {
	outCol = texture(texture0, TexCoord) * Color;
}
)";

const std::string OpenGLShader::sdf_uniforms = R"(
struct Uniforms {
	vec2 texture_size;
	float pixel_range;
};
)";

const std::string OpenGLShader::sdf_fragment = R"(
in vec2 TexCoord;
in vec4 Color;
flat in int Idx;
out vec4 outCol;
uniform sampler2D texture0;

float median(float r, float g, float b) {
	return max(min(r, g), min(max(r, g), b));
}

float screenPxRange(vec2 texCoord, vec2 textureSize, float pxRange) {
	vec2 unitRange = vec2(pxRange)/textureSize;
	vec2 screenTexSize = vec2(1.0)/fwidth(texCoord);
	return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

void main() {
	Uniforms u = uniforms[Idx];
	vec3 msd = texture(texture0, TexCoord).rgb;
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = screenPxRange(TexCoord, u.texture_size, u.pixel_range)*(sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	outCol = vec4(Color.rgb, Color.a * opacity);
}
)";

const std::string OpenGLShader::rect_uniforms = R"(
struct Uniforms {
	vec2 rect_size;
	float border_size;
};
)";

const std::string OpenGLShader::rect_fragment = R"(
in vec2 TexCoord;
in vec4 Color;
flat in int Idx;
out vec4 outCol;

void main() {
	Uniforms u = uniforms[Idx];
	vec2 border_uv = u.border_size / u.rect_size;
	vec2 dist = min(TexCoord, 1.0 - TexCoord);
	float inside = step(border_uv.x, dist.x) * step(border_uv.y, dist.y);
	vec4 fill_color = mix(Color, vec4(0.0), step(1.0, u.border_size));
	outCol = mix(Color, fill_color, inside);
}
)";
