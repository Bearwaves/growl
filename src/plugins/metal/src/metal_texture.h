#pragma once

#include "growl/core/graphics/texture.h"
#include <Metal/Metal.h>

namespace Growl {

class MetalTexture : public Texture {
public:
	MetalTexture(
		id<MTLTexture> metal_texture, id<MTLSamplerState> sampler, int width,
		int height)
		: Texture(width, height)
		, metal_texture{metal_texture}
		, sampler{sampler} {}

	void bind(id<MTLRenderCommandEncoder> encoder) const;

	id<MTLTexture> getRaw() const {
		return metal_texture;
	}

private:
	id<MTLTexture> metal_texture;
	id<MTLSamplerState> sampler;
};

} // namespace Growl
