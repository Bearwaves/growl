#pragma once

#include "growl/core/graphics/texture.h"
#include "opengl.h"

namespace Growl {

class OpenGLTexture : public Texture {
public:
	explicit OpenGLTexture(unsigned int id, int width, int height)
		: Texture(width, height)
		, id{id} {}
	~OpenGLTexture();

	void bind() const;

	GLuint getRaw() const {
		return id;
	}

#ifdef GROWL_IMGUI
	ImTextureID getImguiTextureID() {
		return id;
	};
#endif

private:
	GLuint id;
};

} // namespace Growl
