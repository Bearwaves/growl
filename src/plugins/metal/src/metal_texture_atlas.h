#pragma once

#include <growl/core/graphics/texture_atlas.h>
namespace Growl {

class MetalTextureAtlas : public TextureAtlas {
public:
	MetalTextureAtlas(const Atlas& atlas, std::unique_ptr<Texture> texture) {
		this->texture = std::move(texture);
		this->mappings = atlas.getMappings();
	}

	Result<TextureAtlasRegion> getRegion(const std::string& name) override;
};

} // namespace Growl
