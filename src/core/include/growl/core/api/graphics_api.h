#pragma once

#include "growl/core/error.h"
#include "growl/core/graphics/texture.h"
#include <memory>

namespace Growl {

class Atlas;
class Batch;
class FontFace;
class FontTextureAtlas;
class Image;
class Shader;
class ShaderPack;
class Texture;
class TextureAtlas;

class GraphicsAPI {

public:
	virtual ~GraphicsAPI() {}
	double getDeltaTime() {
		return deltaTime;
	}
	double getTotalTime() {
		return totalTime;
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
	virtual Result<std::unique_ptr<Shader>>
	createShader(const ShaderPack& shader_pack) = 0;

protected:
	double deltaTime = 0;
	double totalTime = 0;
};

} // namespace Growl
