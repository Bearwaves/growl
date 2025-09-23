#pragma once

#include "growl/core/graphics/texture.h"
#include "opengl.h"

namespace Growl {

class Image;

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

	Result<std::unique_ptr<Image>> toImage() override;

#ifdef GROWL_IMGUI
	ImTextureID getImguiTextureID() override {
		return id;
	};
#endif

private:
	GLuint id;
};

} // namespace Growl
