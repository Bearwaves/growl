#include "opengl_texture.h"

using Growl::OpenGLTexture;

OpenGLTexture::~OpenGLTexture() {
	glDeleteTextures(1, &id);
}

void OpenGLTexture::bind() const {
	glBindTexture(GL_TEXTURE_2D, id);
}
