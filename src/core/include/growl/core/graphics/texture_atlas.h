#pragma once

#include "growl/util/assets/atlas.h"
#include "growl/util/error.h"
#include "texture.h"
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
	TextureAtlas(const Atlas& atlas, std::unique_ptr<Texture> texture)
		: texture{std::move(texture)}
		, mappings{atlas.getMappings()} {}

	// TextureAtlas is move-only
	TextureAtlas(const TextureAtlas&) = delete;
	TextureAtlas& operator=(const TextureAtlas&) = delete;
	TextureAtlas(TextureAtlas&&) = default;
	TextureAtlas& operator=(TextureAtlas&&) = default;

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
