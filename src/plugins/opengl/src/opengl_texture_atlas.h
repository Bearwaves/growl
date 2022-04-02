#pragma once

#include <growl/core/graphics/texture_atlas.h>
namespace Growl {

class OpenGLTextureAtlas : public TextureAtlas {
public:
	OpenGLTextureAtlas(const Atlas& atlas, std::unique_ptr<Texture> texture)
		: TextureAtlas(atlas, std::move(texture)) {}

	Result<TextureAtlasRegion> getRegion(const std::string& name) override;
};

} // namespace Growl
