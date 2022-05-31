#pragma once

#include "growl/core/graphics/texture.h"
#include "growl/util/assets/atlas.h"
#include "growl/util/assets/font_atlas.h"
#include <unordered_map>

namespace Growl {

class FontTextureAtlas {
public:
	FontTextureAtlas(
		const FontAtlas& font_atlas, std::unique_ptr<Texture> texture)
		: type{font_atlas.getType()}
		, glyphs{font_atlas.getGlyphs()}
		, texture{std::move(texture)} {}

	virtual ~FontTextureAtlas() = default;

	// TextureAtlas is move-only
	FontTextureAtlas(const FontTextureAtlas&) = delete;
	FontTextureAtlas& operator=(const FontTextureAtlas&) = delete;
	FontTextureAtlas(FontTextureAtlas&&) = default;
	FontTextureAtlas& operator=(FontTextureAtlas&&) = default;

	Result<AtlasRegion> getRegion(int glyph_code) const;

	const FontAtlasType getType() const {
		return type;
	}

	const Texture& getTexture() const {
		return *texture;
	}

private:
	FontAtlasType type;
	std::unordered_map<int, AtlasRegion> glyphs;
	std::unique_ptr<Texture> texture;
};

} // namespace Growl
