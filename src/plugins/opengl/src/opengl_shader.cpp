#include "opengl_shader.h"
#include "opengl_graphics.h"

using Growl::OpenGLShader;

OpenGLShader::OpenGLShader(OpenGLGraphicsAPI& graphics) {
	GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
	const char* vertexSource = DEFAULT_VERTEX.c_str();
	glShaderSource(vertex, 1, &vertexSource, nullptr);
	glCompileShader(vertex);
	graphics.checkShaderCompileError(vertex);

	GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
	const char* fragmentSource = DEFAULT_FRAGMENT.c_str();
	glShaderSource(fragment, 1, &fragmentSource, nullptr);
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

void OpenGLShader::bind(glm::mat4 mvp) {
	glUseProgram(program);
	GLint posAttrib = glGetAttribLocation(program, "position");
	glEnableVertexAttribArray(posAttrib);
	glVertexAttribPointer(
		posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	GLint texAttrib = glGetAttribLocation(program, "texCoord");
	glEnableVertexAttribArray(texAttrib);
	glVertexAttribPointer(
		texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat),
		(void*)(2 * sizeof(GLfloat)));
	GLuint mvpID = glGetUniformLocation(program, "mvp");
	glUniformMatrix4fv(mvpID, 1, GL_FALSE, glm::value_ptr(mvp));
}

std::string const OpenGLShader::DEFAULT_VERTEX = R"(
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

std::string const OpenGLShader::DEFAULT_FRAGMENT = R"(
#version 150 core

in vec2 TexCoord;
out vec4 outCol;
uniform sampler2D texture0;

void main() {
	outCol = texture(texture0, TexCoord);
}
)";
