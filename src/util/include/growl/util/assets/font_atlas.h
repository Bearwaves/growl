#pragma once

#include "atlas.h"
#include "font.h"
#include "image.h"
#include "nlohmann/json.hpp"
#include <memory>
#include <unordered_map>

using nlohmann::json;

namespace Growl {

enum class FontAtlasType { RGBA, SDF, MSDF };

class FontAtlas {
public:
	FontAtlas(
		FontAtlasType type, Font& font, std::unique_ptr<Image> img,
		std::unordered_map<int, AtlasRegion> glyphs)
		: type{type}
		, font{font}
		, img{std::move(img)}
		, glyphs{std::move(glyphs)} {}

	const Font& getFont() const {
		return font;
	}

	const Image& getImage() const {
		return *img;
	};

	const FontAtlasType getType() const {
		return type;
	}

	const std::unordered_map<int, AtlasRegion> getGlyphs() const {
		return glyphs;
	}

private:
	FontAtlasType type;
	Font& font;
	std::unique_ptr<Image> img;
	std::unordered_map<int, AtlasRegion> glyphs;
};

Result<FontAtlas> createFontAtlasFromFont(Font& font, int size) noexcept;
Result<FontAtlas>
createFontAtlasFromFont(Font& font, int size, std::string text) noexcept;
Result<FontAtlas>
createDistanceFieldFontAtlasFromFont(Font& font, int size) noexcept;
} // namespace Growl
