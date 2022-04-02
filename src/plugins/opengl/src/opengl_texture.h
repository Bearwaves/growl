#pragma once

#include "opengl.h"
#include <growl/core/graphics/texture.h>

namespace Growl {

class OpenGLTexture : public Texture {
public:
	explicit OpenGLTexture(unsigned int id, int width, int height)
		: Texture(width, height)
		, id{id} {}
	~OpenGLTexture();
	void bind() const;

private:
	GLuint id;
};

} // namespace Growl
