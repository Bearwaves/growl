#include "opengl_graphics.h"
#include "opengl_error.h"
#ifdef GROWL_SDL2
#include "SDL_video.h"
#endif
#ifdef GROWL_ANDROID
#include <EGL/egl.h>
#endif
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/mat4x4.hpp"
#include "growl/core/assets/font_face.h"
#ifdef GROWL_IMGUI
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#endif
#include "opengl.h"
#include "opengl_batch.h"
#include "opengl_shader.h"
#include "opengl_texture.h"
#include "opengl_texture_atlas.h"
#include <memory>
#include <vector>

using Growl::Atlas;
using Growl::Batch;
using Growl::Error;
using Growl::FontTextureAtlas;
using Growl::OpenGLError;
using Growl::OpenGLGraphicsAPI;
using Growl::Texture;
using Growl::TextureAtlas;
using Growl::TextureOptions;
using std::chrono::duration;
using std::chrono::seconds;

OpenGLGraphicsAPI::OpenGLGraphicsAPI(API& api)
	: api{api} {}

Error OpenGLGraphicsAPI::init() {
	last_render = high_resolution_clock::now();
	return nullptr;
}

void OpenGLGraphicsAPI::dispose() {
#ifdef GROWL_IMGUI
	ImGui_ImplOpenGL3_Shutdown();
#endif
#ifdef GROWL_SDL2
	SDL_GL_DeleteContext(context);
#endif
#ifdef GROWL_ANDROID
	if (display != EGL_NO_DISPLAY) {
		eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (context != EGL_NO_CONTEXT) {
			eglDestroyContext(display, context);
		}
		if (surface != EGL_NO_SURFACE) {
			eglDestroySurface(display, surface);
		}
		eglTerminate(display);
	}
#endif
}

void OpenGLGraphicsAPI::begin() {
	auto tp = high_resolution_clock::now();
	deltaTime = duration<double, seconds::period>(tp - last_render).count();
	last_render = tp;

#ifdef GROWL_IMGUI
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
#endif
}

void OpenGLGraphicsAPI::end() {
#ifdef GROWL_IMGUI
	ImGui::Render();
	if (api.imguiVisible()) {
		int w, h;
		SDL_GetWindowSize(
			static_cast<SDL_Window*>(window->getNative()), &w, &h);
		glViewport(0, 0, w, h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
#endif
#ifdef GROWL_SDL2
	SDL_GL_SwapWindow(static_cast<SDL_Window*>(window->getNative()));
#endif
#ifdef GROWL_ANDROID
	eglSwapBuffers(display, surface);
#endif
}

Error OpenGLGraphicsAPI::setWindow(const WindowConfig& config) {
	auto window_result = api.system().createWindow(config);
	if (window_result.hasError()) {
		return std::move(window_result.error());
	}
	window = std::move(window_result.get());

#ifdef GROWL_SDL2
#ifdef GROWL_OPENGL_4_1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#elif GROWL_OPENGL_4_5
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
#endif
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(
		SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	context =
		SDL_GL_CreateContext(static_cast<SDL_Window*>(window->getNative()));
	glViewport(0, 0, config.getWidth(), config.getHeight());
#endif
#ifdef GROWL_OPENGL_4_5
	gladLoadGLLoader(SDL_GL_GetProcAddress);
#endif

#ifdef GROWL_ANDROID
	const EGLint attribs[] = {EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
							  EGL_BLUE_SIZE,	8,
							  EGL_GREEN_SIZE,	8,
							  EGL_RED_SIZE,		8,
							  EGL_NONE};
	EGLint num_configs;
	EGLConfig egl_config = nullptr;
	display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(display, nullptr, nullptr);

	// Choose the best config
	eglChooseConfig(display, attribs, nullptr, 0, &num_configs);
	std::unique_ptr<EGLConfig[]> supported_configs(new EGLConfig[num_configs]);
	eglChooseConfig(
		display, attribs, supported_configs.get(), num_configs, &num_configs);
	if (!num_configs) {
		return std::make_unique<OpenGLError>("Failed to find EGL configs");
	}
	auto i = 0;
	for (; i < num_configs; i++) {
		auto& cfg = supported_configs[i];
		EGLint r, g, b, d;
		if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r) &&
			eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
			eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b) &&
			eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) && r == 8 &&
			g == 8 && b == 8 && d == 0) {

			egl_config = supported_configs[i];
			break;
		}
	}
	if (i == num_configs) {
		egl_config = supported_configs[0];
	}

	// todo handle error
	surface = eglCreateWindowSurface(
		display, egl_config, static_cast<ANativeWindow*>(window->getNative()),
		nullptr);
	const EGLint context_attribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
	context = eglCreateContext(display, egl_config, nullptr, context_attribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		return std::make_unique<OpenGLError>(
			"Failed to get an EGL display: code " +
			std::to_string(eglGetError()));
	}

	EGLint w, h;
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);
	glViewport(0, 0, w, h);
#endif

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	api.system().log(
		"OpenGLGraphicsAPI", "Loaded OpenGL version {}.{}", major, minor);

#ifdef GROWL_IMGUI
	ImGui_ImplSDL2_InitForOpenGL(
		static_cast<SDL_Window*>(window->getNative()), context);
	ImGui_ImplOpenGL3_Init("#version 150 core");
#endif

	default_shader = std::make_unique<OpenGLShader>(*this);
	sdf_shader = std::make_unique<OpenGLShader>(
		*this, OpenGLShader::default_vertex, OpenGLShader::sdf_fragment);
	rect_shader = std::make_unique<OpenGLShader>(
		*this, OpenGLShader::default_vertex, OpenGLShader::rect_fragment);
	setupDebugCallback();

	return nullptr;
}

void OpenGLGraphicsAPI::clear(float r, float g, float b) {
	glClearColor(r, g, b, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::createTexture(
	const Image& image, const TextureOptions options) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, image.getRaw());

	return setupTexture(
		texture_id, image.getWidth(), image.getHeight(), options);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::createTexture(
	unsigned int width, unsigned int height, const TextureOptions options) {
	unsigned int texture_id;
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);

	std::vector<unsigned char> data(4 * width * height, 0);

	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		data.data());

	return setupTexture(texture_id, width, height, options);
}

std::unique_ptr<Texture> OpenGLGraphicsAPI::setupTexture(
	unsigned int texture_id, int width, int height,
	const TextureOptions options) {
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if (options.mipmapped) {
		glGenerateMipmap(GL_TEXTURE_2D);
		if (options.filtering) {
			glTexParameteri(
				GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		} else {
			glTexParameteri(
				GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
				GL_NEAREST_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
	} else if (options.filtering) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	return std::make_unique<OpenGLTexture>(texture_id, width, height);
}

std::unique_ptr<TextureAtlas> OpenGLGraphicsAPI::createTextureAtlas(
	const Atlas& atlas, const TextureOptions options) {
	return std::make_unique<OpenGLTextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<FontTextureAtlas>
OpenGLGraphicsAPI::createFontTextureAtlas(const FontFace& face) {
	bool is_msdf = face.getType() != FontFaceType::Bitmap;
	return std::make_unique<FontTextureAtlas>(
		face, createTexture(face.getImage(), {is_msdf, is_msdf}));
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch() {
	int w, h;
#ifdef GROWL_SDL2
	SDL_GL_GetDrawableSize(
		static_cast<SDL_Window*>(window->getNative()), &w, &h);
#endif
#ifdef GROWL_ANDROID
	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);
#endif
	glViewport(0, 0, w, h);
	auto projection = glm::ortho<float>(
		0, static_cast<float>(w), static_cast<float>(h), 0, 1, -1);
	return std::make_unique<OpenGLBatch>(
		default_shader.get(), sdf_shader.get(), rect_shader.get(), projection,
		w, h, 0);
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch(const Texture& texture) {
	auto& opengl_texture = static_cast<const OpenGLTexture&>(texture);
	glViewport(0, 0, texture.getWidth(), texture.getHeight());
	auto projection = glm::ortho<float>(
		0, static_cast<float>(texture.getWidth()), 0,
		static_cast<float>(texture.getHeight()), 1, -1);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		opengl_texture.getRaw(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_unique<OpenGLBatch>(
		default_shader.get(), sdf_shader.get(), rect_shader.get(), projection,
		texture.getWidth(), texture.getHeight(), fbo);
}

void OpenGLGraphicsAPI::onWindowResize(int width, int height) {}

void OpenGLGraphicsAPI::checkGLError(const char* file, long line) {
	int err = glGetError();
	if (err) {
		api.system().log(
			LogLevel::ERROR, "OpenGL", "Error {:#04x} at {}:{}", err, file,
			line);
	};
}

void OpenGLGraphicsAPI::checkShaderCompileError(unsigned int shader) {
	int result;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int info_len;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
		char* log = new char[info_len];
		glGetShaderInfoLog(shader, info_len, &info_len, log);
		api.system().log(
			LogLevel::ERROR, "OpenGL", "Error compiling shader: {}", log);
		delete[] log;
	}
}

void OpenGLGraphicsAPI::setupDebugCallback() {
#ifdef GROWL_OPENGL_4_5
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(
		[](GLenum source, GLenum type, GLuint id, GLenum severity,
		   GLsizei length, const GLchar* message, const void* user_param) {
			reinterpret_cast<const OpenGLGraphicsAPI*>(user_param)
				->onGLDebugMessage(source, type, id, severity, message);
		},
		this);
	checkGLError(__FILE__, __LINE__);
#endif
}

void OpenGLGraphicsAPI::onGLDebugMessage(
	GLenum source, GLenum type, GLuint id, GLenum severity,
	const GLchar* message) const {
#ifdef GROWL_OPENGL_4_5
	if (severity == GL_DEBUG_SEVERITY_HIGH ||
		severity == GL_DEBUG_SEVERITY_MEDIUM) {
		api.system().log(LogLevel::ERROR, "OpenGL", "{}", message);
	}
#endif
}
