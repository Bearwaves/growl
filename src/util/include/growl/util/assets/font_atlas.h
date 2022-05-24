#pragma once

#include "font.h"
#include "image.h"
#include <memory>
#include <unordered_map>

namespace Growl {

struct GlyphPosition {
	float u0; // left
	float v0; // top
	float u1; // right
	float v1; // bottom
};

enum class FontAtlasType { RGBA, SDF, MSDF };

class FontAtlas {
public:
	FontAtlas(
		FontAtlasType type, Font& font, std::unique_ptr<Image> img,
		std::unordered_map<int, GlyphPosition> glyphs)
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

	const std::unordered_map<int, GlyphPosition> getGlyphs() const {
		return glyphs;
	}

private:
	FontAtlasType type;
	Font& font;
	std::unique_ptr<Image> img;
	std::unordered_map<int, GlyphPosition> glyphs;
};

Result<FontAtlas> createFontAtlasFromFont(Font& font, int size) noexcept;
Result<FontAtlas>
createFontAtlasFromFont(Font& font, int size, std::string text) noexcept;
Result<FontAtlas> createDistanceFieldFontAtlasFromFont(Font& font) noexcept;
} // namespace Growl
