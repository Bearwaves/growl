#pragma once

#include "texture.h"
#include <growl/util/assets/atlas.h>
#include <growl/util/error.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace Growl {

class TextureAtlas;

struct TextureAtlasRegion {
	TextureAtlas* atlas;
	AtlasRegion region;
};

class TextureAtlas {
public:
	virtual ~TextureAtlas() = default;
	virtual Result<TextureAtlasRegion> getRegion(const std::string& name) = 0;

	const Texture& getTexture() const {
		return *texture;
	}

protected:
	std::unique_ptr<Texture> texture;
	std::unordered_map<std::string, AtlasRegion> mappings;
};

} // namespace Growl
