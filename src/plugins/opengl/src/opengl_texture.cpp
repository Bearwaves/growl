#include "opengl_texture.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"

using Growl::Image;
using Growl::OpenGLTexture;
using Growl::Result;

OpenGLTexture::~OpenGLTexture() {
	glDeleteTextures(1, &id);
}

void OpenGLTexture::bind() const {
	glBindTexture(GL_TEXTURE_2D, id);
}

Result<std::unique_ptr<Image>> OpenGLTexture::toImage() {
	std::vector<unsigned char> data(width * height * 4, 0);
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

	return std::make_unique<Image>(width, height, 4, std::move(data));
}
