#pragma once

#include <Metal/Metal.h>
#include <growl/core/graphics/texture.h>

namespace Growl {

class MetalTexture : public Texture {
public:
	MetalTexture(id<MTLTexture> metalTexture, id<MTLSamplerState> sampler)
		: metalTexture{metalTexture}
		, sampler{sampler} {}
	void bind(id<MTLRenderCommandEncoder> encoder);

private:
	id<MTLTexture> metalTexture;
	id<MTLSamplerState> sampler;
};

} // namespace Growl
