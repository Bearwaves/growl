#include "metal_texture.h"

using Growl::MetalTexture;

void MetalTexture::bind(id<MTLRenderCommandEncoder> encoder) {
	[encoder setFragmentTexture:metalTexture atIndex:0];
	[encoder setFragmentSamplerState:sampler atIndex:0];
}
