#include "opengl_graphics.h"
#include "opengl_batch.h"
#include "opengl_texture.h"
#include <growl/core/log.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#include <SDL_opengl.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/mat4x4.hpp>
#include <memory>

using Growl::Batch;
using Growl::Error;
using Growl::OpenGLGraphicsAPI;
using Growl::Texture;
using std::chrono::duration;
using std::chrono::seconds;

OpenGLGraphicsAPI::OpenGLGraphicsAPI(SystemAPI& system)
	: system{system} {}

Error OpenGLGraphicsAPI::init() {
	last_render = high_resolution_clock::now();
	return nullptr;
}

void OpenGLGraphicsAPI::dispose() {
	SDL_GL_DeleteContext(context);
}

void OpenGLGraphicsAPI::begin() {
	auto tp = high_resolution_clock::now();
	deltaTime = duration<double, seconds::period>(tp - last_render).count();
	last_render = tp;
}

void OpenGLGraphicsAPI::end() {
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(window->getNative()));
}

void OpenGLGraphicsAPI::setWindow(WindowConfig& config) {
	window = system.createWindow(config);
	context =
		SDL_GL_CreateContext(static_cast<SDL_Window*>(window->getNative()));
	glViewport(0, 0, config.getWidth(), config.getHeight());
	default_shader = std::make_unique<OpenGLShader>(*this);
	setupDebugCallback();
}

void OpenGLGraphicsAPI::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::createTexture(const Image& image) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(
		GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image.getRaw());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	return std::make_unique<OpenGLTexture>(textureID);
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch() {
	int w, h;
	SDL_GetWindowSize(static_cast<SDL_Window*>(window->getNative()), &w, &h);
	glViewport(0, 0, w, h);
	auto projection = glm::ortho<float>(0, w, h, 0, 1, -1);
	return std::make_unique<OpenGLBatch>(default_shader.get(), projection);
}

void OpenGLGraphicsAPI::checkGLError(const char* file, long line) {
	int err = glGetError();
	if (err) {
		system.log(
			LogLevel::ERROR, "OpenGL", "Error {:#04x} at {}:{}", err, file,
			line);
	};
}

void OpenGLGraphicsAPI::checkShaderCompileError(unsigned int shader) {
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int infoLen;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		char* log = new char[infoLen];
		glGetShaderInfoLog(shader, infoLen, &infoLen, log);
		system.log(
			LogLevel::ERROR, "OpenGL", "Error compiling shader: {}", log);
		delete[] log;
	}
}

void OpenGLGraphicsAPI::setupDebugCallback() {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(
		[](GLenum source, GLenum type, GLuint id, GLenum severity,
		   GLsizei length, const GLchar* message, const void* userParam) {
			reinterpret_cast<const OpenGLGraphicsAPI*>(userParam)
				->onGLDebugMessage(source, type, id, severity, message);
		},
		this);
	checkGLError(__FILE__, __LINE__);
}

void OpenGLGraphicsAPI::onGLDebugMessage(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	const GLchar* message) const {
	if (severity == GL_DEBUG_SEVERITY_HIGH ||
		severity == GL_DEBUG_SEVERITY_MEDIUM) {
		system.log(LogLevel::ERROR, "OpenGL", "{}", message);
	}
}
