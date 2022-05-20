#include "opengl_shader.h"
#include "growl/core/graphics/color.h"
#include "opengl_graphics.h"

using Growl::OpenGLShader;

OpenGLShader::OpenGLShader(
	OpenGLGraphicsAPI& graphics, std::string vertex_src,
	std::string fragment_src) {
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	const char* vertex_source = vertex_src.c_str();
	glShaderSource(vertex, 1, &vertex_source, nullptr);
	glCompileShader(vertex);
	graphics.checkShaderCompileError(vertex);

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragment_source = fragment_src.c_str();
	glShaderSource(fragment, 1, &fragment_source, nullptr);
	glCompileShader(fragment);
	graphics.checkShaderCompileError(fragment);

	program = glCreateProgram();
	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

OpenGLShader::~OpenGLShader() {
	glDeleteProgram(program);
}

void OpenGLShader::bind(glm::mat4 mvp, Color color) {
	glUseProgram(program);
	GLint pos_attrib = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(pos_attrib);
	glVertexAttribPointer(
		pos_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	GLint tex_attrib = glGetAttribLocation(program, "texCoord");
	if (tex_attrib >= 0) {
		glEnableVertexAttribArray(tex_attrib);
		glVertexAttribPointer(
			tex_attrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
			(void*)(2 * sizeof(GLfloat)));
	}
	GLuint mvp_id = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(mvp_id, 1, GL_FALSE, glm::value_ptr(mvp));
	GLuint color_id = glGetUniformLocation(program, "color");
	glUniform4f(color_id, color.r, color.g, color.b, color.a);
}

std::string const OpenGLShader::default_vertex = R"(
#version 150 core

in vec2 position;
in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 mvp;

void main() {
	TexCoord = texCoord;
	gl_Position = mvp * vec4(position, 0, 1);
}
)";

std::string const OpenGLShader::default_fragment = R"(
#version 150 core

in vec2 TexCoord;
out vec4 outCol;
uniform sampler2D texture0;
uniform vec4 color;

void main() {
	outCol = texture(texture0, TexCoord) * color;
}
)";

std::string const OpenGLShader::sdf_fragment = R"(
#version 150 core

in vec2 TexCoord;
out vec4 outCol;
uniform sampler2D texture0;
uniform vec4 color;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
	vec4 msd = texture(texture0, TexCoord).rgba;
	float sd = median(msd.r, msd.g, msd.b);
	float screenPxDistance = 2 *(sd - 0.5);
	float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);
	outCol = mix(vec4(1, 1, 1, 0), vec4(1, 1, 1, 1), opacity) * color;
}
)";

std::string const OpenGLShader::rect_fragment = R"(
#version 150 core

out vec4 outCol;
uniform vec4 color;

void main() {
	outCol = color;
}
)";
