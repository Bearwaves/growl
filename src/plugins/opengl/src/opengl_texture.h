#pragma once

#include <GLES3/gl3.h>
#include <growl/core/graphics/texture.h>

namespace Growl {

class OpenGLTexture : public Texture {
public:
	explicit OpenGLTexture(unsigned int id)
		: id{id} {}
	~OpenGLTexture();
	void bind();

private:
	GLuint id;
};

} // namespace Growl
