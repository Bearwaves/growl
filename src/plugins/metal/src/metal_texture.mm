#include "metal_texture.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"
#include <Metal/Metal.h>
#include <vector>
#ifdef GROWL_IMGUI
#include "imgui.h"
#endif

using Growl::Image;
using Growl::MetalTexture;
using Growl::Result;

void MetalTexture::bind(id<MTLRenderCommandEncoder> encoder) const {
	[encoder setFragmentTexture:metal_texture atIndex:0];
	[encoder setFragmentSamplerState:sampler atIndex:0];
}

Result<std::unique_ptr<Image>> MetalTexture::toImage() {
	int bytes_per_row = 4 * width;
	int bytes_total = bytes_per_row * height;
	std::vector<unsigned char> data(bytes_total, 0);
	[metal_texture getBytes:data.data()
				bytesPerRow:bytes_per_row
				 fromRegion:MTLRegionMake2D(0, 0, width, height)
				mipmapLevel:0];

	for (int i = 0; i < width * height; i++) {
		std::swap(data[i * 4], data[i * 4 + 2]);
	}

	return std::make_unique<Image>(width, height, 4, std::move(data));
}

#ifdef GROWL_IMGUI
ImTextureID MetalTexture::getImguiTextureID() {
	return reinterpret_cast<ImTextureID>(metal_texture);
}
#endif
