#include "metal_texture_atlas.h"
#include <growl/util/assets/error.h>

using Growl::Error;
using Growl::MetalTextureAtlas;
using Growl::Result;
using Growl::TextureAtlasRegion;

Result<TextureAtlasRegion>
MetalTextureAtlas::getRegion(const std::string& name) {
	if (auto it = mappings.find(name); it != mappings.end()) {
		return TextureAtlasRegion{this, it->second};
	}
	return Error(std::make_unique<AssetsError>(
		"Failed to find region " + name + " in atlas."));
}
