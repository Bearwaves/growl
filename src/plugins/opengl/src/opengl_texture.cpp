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

Result<std::unique_ptr<Image>> OpenGLTexture::toImage(int channels) {
	int gl_channels = channels;
#ifdef GROWL_OPENGL_ES
	gl_channels = 4;
#else
	if (channels != 3) {
		gl_channels = 4;
	}
#endif
	std::vector<unsigned char> data(width * height * gl_channels, 0);
	GLuint fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id, 0);
	glReadPixels(
		0, 0, width, height, gl_channels == 3 ? GL_RGB : GL_RGBA,
		GL_UNSIGNED_BYTE, data.data());

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &fbo);

#ifdef GROWL_OPENGL_ES
	// OpenGL ES doesn't support GL_RGB so we must transform it here.
	if (channels == 3) {
		std::vector<unsigned char> rgb_data(3 * width * height, 0);
		int j = 0;
		for (int i = 0; i < data.size(); i++) {
			rgb_data[j++] = data[i++];
			rgb_data[j++] = data[i++];
			rgb_data[j++] = data[i++];
		}
		return std::make_unique<Image>(width, height, 3, std::move(rgb_data));
	}
#endif
	return std::make_unique<Image>(width, height, channels, std::move(data));
}
