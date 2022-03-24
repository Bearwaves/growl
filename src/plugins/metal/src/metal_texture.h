#pragma once

#include <Metal/Metal.h>
#include <growl/core/graphics/texture.h>

namespace Growl {

class MetalTexture : public Texture {
public:
	MetalTexture(
		id<MTLTexture> metalTexture, id<MTLSamplerState> sampler, int width,
		int height)
		: metalTexture{metalTexture}
		, sampler{sampler} {
		this->width = width;
		this->height = height;
	}
	void bind(id<MTLRenderCommandEncoder> encoder) const;

private:
	id<MTLTexture> metalTexture;
	id<MTLSamplerState> sampler;
};

} // namespace Growl
