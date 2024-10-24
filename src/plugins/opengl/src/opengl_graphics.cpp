#include "opengl_graphics.h"
#include "growl/core/api/api.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/shader_pack.h"
#include "growl/core/error.h"
#include "growl/core/graphics/shader.h"
#include "opengl_error.h"
#ifdef GROWL_IMGUI
#include "growl/core/imgui.h"
#include "imgui_impl_opengl3.h"
#endif
#include "opengl.h"
#include "opengl_batch.h"
#include "opengl_shader.h"
#include "opengl_texture.h"
#include <memory>
#include <vector>

using Growl::Atlas;
using Growl::Batch;
using Growl::Error;
using Growl::FontTextureAtlas;
using Growl::OpenGLGraphicsAPI;
using Growl::Result;
using Growl::Shader;
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
}

void OpenGLGraphicsAPI::begin() {
#ifdef GROWL_IMGUI
	ImGui_ImplOpenGL3_NewFrame();
	window->newImguiFrame();
	imGuiBegin();
#endif
}

void OpenGLGraphicsAPI::end() {
#ifdef GROWL_IMGUI
	imGuiEnd();
	if (api.imguiVisible()) {
		int w, h;
		window->getSize(&w, &h);
		glViewport(0, 0, w, h);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
#endif
	window->flip();
}

Error OpenGLGraphicsAPI::setWindow(const Config& config) {
	auto window_result = api.system().createWindow(config);
	if (window_result.hasError()) {
		return std::move(window_result.error());
	}
	window = std::move(window_result.get());

#ifdef GROWL_OPENGL_4_1
	if (auto err = window->createGLContext(4, 1)) {
		return err;
	}
#elif GROWL_OPENGL_4_5
	if (auto err = window->createGLContext(4, 5)) {
		return err;
	}
	gladLoadGLLoader(window->glLibraryLoaderFunc());
#elif GROWL_OPENGL_ES
	if (auto err = window->createGLContext(3, 0, true)) {
		return err;
	}
#endif

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	api.system().log(
		"OpenGLGraphicsAPI", "Loaded OpenGL version {}.{}", major, minor);

#ifdef GROWL_IMGUI
	window->initImgui();
#ifdef GROWL_OPENGL_ES
	ImGui_ImplOpenGL3_Init("#version 300 es");
#else
	ImGui_ImplOpenGL3_Init("#version 150 core");
#endif
#endif

	default_shader = std::make_unique<OpenGLShader>(
		OpenGLShader::default_uniforms, OpenGLShader::default_vertex,
		OpenGLShader::default_fragment);
	if (auto err = default_shader->compile()) {
		return err;
	}
	sdf_shader = std::make_unique<OpenGLShader>(
		OpenGLShader::default_uniforms, OpenGLShader::default_vertex,
		OpenGLShader::sdf_fragment);
	if (auto err = sdf_shader->compile()) {
		return err;
	}
	rect_shader = std::make_unique<OpenGLShader>(
		OpenGLShader::rect_uniforms, OpenGLShader::default_vertex,
		OpenGLShader::rect_fragment);
	if (auto err = rect_shader->compile()) {
		return err;
	}
	setupDebugCallback();

	return nullptr;
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
	return std::make_unique<TextureAtlas>(
		atlas, createTexture(atlas.getImage(), options));
}

std::unique_ptr<FontTextureAtlas>
OpenGLGraphicsAPI::createFontTextureAtlas(const FontFace& face) {
	bool is_msdf = face.getType() != FontFaceType::Bitmap;
	return std::make_unique<FontTextureAtlas>(
		face, createTexture(face.getImage(), {is_msdf, false}));
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch() {
	int w, h;
	window->getSize(&w, &h);
	return std::make_unique<OpenGLBatch>(
		&api, default_shader.get(), sdf_shader.get(), rect_shader.get(), w, h,
		window.get(), 0);
}

std::unique_ptr<Batch> OpenGLGraphicsAPI::createBatch(const Texture& texture) {
	auto& opengl_texture = static_cast<const OpenGLTexture&>(texture);

	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		opengl_texture.getRaw(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return std::make_unique<OpenGLBatch>(
		&api, default_shader.get(), sdf_shader.get(), rect_shader.get(),
		texture.getWidth(), texture.getHeight(), nullptr, fbo);
}

Result<std::unique_ptr<Shader>> OpenGLGraphicsAPI::createShader(
	const std::string& uniforms_src, const std::string& vert_src,
	const std::string& fragment_src) {
	auto shader =
		std::make_unique<OpenGLShader>(uniforms_src, vert_src, fragment_src);
	if (auto err = shader->compile()) {
		return err;
	}

	return std::unique_ptr<Shader>(std::move(shader));
}

Result<std::unique_ptr<Shader>> OpenGLGraphicsAPI::createShader(
	const std::string& uniforms_src, const std::string& fragment_src) {
	return createShader(
		uniforms_src, OpenGLShader::default_vertex, fragment_src);
}

Result<std::unique_ptr<Shader>>
OpenGLGraphicsAPI::createShader(const std::string& fragment_src) {
	return createShader(
		OpenGLShader::default_uniforms, OpenGLShader::default_vertex,
		fragment_src);
}

Result<std::unique_ptr<Shader>>
OpenGLGraphicsAPI::createShader(const ShaderPack& shader_pack) {
	auto source = shader_pack.getSources().find(ShaderType::GLSL);
	if (source == shader_pack.getSources().end()) {
		return Error(std::make_unique<OpenGLError>(
			"No GLSL sources found in shader pack"));
	}
	if (!source->second.fragment_src.has_value()) {
		return Error(std::make_unique<OpenGLError>(
			"No fragment source found in shader pack"));
	}
	return createShader(
		source->second.uniforms_src.has_value()
			? source->second.uniforms_src.value()
			: OpenGLShader::default_uniforms,
		source->second.vertex_src.has_value()
			? source->second.vertex_src.value()
			: OpenGLShader::default_vertex,
		source->second.fragment_src.value());
}

void OpenGLGraphicsAPI::onWindowResize(int width, int height) {}

void OpenGLGraphicsAPI::checkGLError(const char* file, long line) {
	int err = glGetError();
	if (err) {
		api.system().log(
			LogLevel::Error, "OpenGL", "Error {:#04x} at {}:{}", err, file,
			line);
	};
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
	Growl::LogLevel level = (severity == GL_DEBUG_SEVERITY_HIGH ||
							 severity == GL_DEBUG_SEVERITY_MEDIUM)
								? LogLevel::Error
								: LogLevel::Debug;
	api.system().log(level, "OpenGL", "{}", message);
#endif
}
