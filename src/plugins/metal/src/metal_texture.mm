#include "metal_texture.h"
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::MetalTexture;

void MetalTexture::bind(id<MTLRenderCommandEncoder> encoder) const {
	[encoder setFragmentTexture:metal_texture atIndex:0];
	[encoder setFragmentSamplerState:sampler atIndex:0];
}

#ifdef GROWL_IMGUI
ImTextureID MetalTexture::getImguiTextureID() {
	return reinterpret_cast<ImTextureID>(metal_texture);
}
#endif
