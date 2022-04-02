#pragma once

#include "../graphics/batch.h"
#include "../graphics/texture.h"
#include "../graphics/texture_atlas.h"
#include "../graphics/window.h"
#include <growl/util/assets/image.h>
#include <memory>

namespace Growl {

class GraphicsAPI {

public:
	virtual ~GraphicsAPI() {}
	double getDeltaTime() {
		return deltaTime;
	}
	virtual void clear(float r, float g, float b) = 0;

	virtual std::unique_ptr<Texture> createTexture(
		const Image& image,
		const TextureOptions options = TextureOptions{}) = 0;
	virtual std::unique_ptr<Texture> createTexture(
		unsigned int width, unsigned int height,
		const TextureOptions options = TextureOptions{}) = 0;

	virtual std::unique_ptr<TextureAtlas> createTextureAtlas(
		const Atlas& atlas,
		const TextureOptions options = TextureOptions{}) = 0;

	virtual std::unique_ptr<Batch> createBatch() = 0;
	virtual std::unique_ptr<Batch> createBatch(const Texture& texture) = 0;

protected:
	double deltaTime = 0;
};

} // namespace Growl
