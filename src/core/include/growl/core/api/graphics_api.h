#pragma once

#include "../graphics/batch.h"
#include "../graphics/font_texture_atlas.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../graphics/texture_atlas.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/image.h"
#include <memory>

namespace Growl {

class GraphicsAPI {

public:
	virtual ~GraphicsAPI() {}
	double getDeltaTime() {
		return deltaTime;
	}

	virtual std::unique_ptr<Texture> createTexture(
		const Image& image,
		const TextureOptions options = TextureOptions{}) = 0;
	virtual std::unique_ptr<Texture> createTexture(
		unsigned int width, unsigned int height,
		const TextureOptions options = TextureOptions{}) = 0;

	virtual std::unique_ptr<TextureAtlas> createTextureAtlas(
		const Atlas& atlas,
		const TextureOptions options = TextureOptions{}) = 0;

	virtual std::unique_ptr<FontTextureAtlas>
	createFontTextureAtlas(const FontFace& face) = 0;

	virtual std::unique_ptr<Batch> createBatch() = 0;
	virtual std::unique_ptr<Batch> createBatch(const Texture& texture) = 0;

	virtual Result<std::unique_ptr<Shader>> createShader(
		const std::string& vertex_src, const std::string& fragment_src) = 0;
	virtual Result<std::unique_ptr<Shader>>
	createShader(const std::string& fragment_src) = 0;

protected:
	double deltaTime = 0;
};

} // namespace Growl
