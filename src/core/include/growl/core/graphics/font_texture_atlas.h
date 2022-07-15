#pragma once

#include "growl/core/graphics/texture.h"
#include "growl/core/assets/atlas.h"
#include "growl/core/assets/font_face.h"
#include <unordered_map>

namespace Growl {

class FontTextureAtlas {
public:
	FontTextureAtlas(
		const FontFace& font_face, std::unique_ptr<Texture> texture)
		: type{font_face.getType()}
		, glyphs{font_face.getGlyphs()}
		, texture{std::move(texture)} {}

	virtual ~FontTextureAtlas() = default;

	// TextureAtlas is move-only
	FontTextureAtlas(const FontTextureAtlas&) = delete;
	FontTextureAtlas& operator=(const FontTextureAtlas&) = delete;
	FontTextureAtlas(FontTextureAtlas&&) = default;
	FontTextureAtlas& operator=(FontTextureAtlas&&) = default;

	Result<AtlasRegion> getRegion(int glyph_code) const;

	const FontFaceType getType() const {
		return type;
	}

	const Texture& getTexture() const {
		return *texture;
	}

private:
	FontFaceType type;
	std::unordered_map<int, AtlasRegion> glyphs;
	std::unique_ptr<Texture> texture;
};

} // namespace Growl
