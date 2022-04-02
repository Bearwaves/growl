#include "metal_texture.h"

using Growl::MetalTexture;

void MetalTexture::bind(id<MTLRenderCommandEncoder> encoder) const {
	[encoder setFragmentTexture:metal_texture atIndex:0];
	[encoder setFragmentSamplerState:sampler atIndex:0];
}
