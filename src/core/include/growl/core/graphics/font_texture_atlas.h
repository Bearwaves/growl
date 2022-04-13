#pragma once

#include "growl/core/graphics/texture.h"
#include "growl/util/assets/font_atlas.h"
#include <unordered_map>

namespace Growl {

class FontTextureAtlas {
public:
	FontTextureAtlas(
		const FontAtlas& font_atlas, std::unique_ptr<Texture> texture)
		: glyphs{font_atlas.getGlyphs()}
		, texture{std::move(texture)} {}

	virtual ~FontTextureAtlas() = default;

	// TextureAtlas is move-only
	FontTextureAtlas(const FontTextureAtlas&) = delete;
	FontTextureAtlas& operator=(const FontTextureAtlas&) = delete;
	FontTextureAtlas(FontTextureAtlas&&) = default;
	FontTextureAtlas& operator=(FontTextureAtlas&&) = default;

	Result<GlyphPosition> getRegion(int glyph_code) const;

	const Texture& getTexture() const {
		return *texture;
	}

private:
	std::unordered_map<int, GlyphPosition> glyphs;
	std::unique_ptr<Texture> texture;
};

} // namespace Growl
