#pragma once

#include "font.h"
#include "image.h"
#include <memory>
#include <unordered_map>

namespace Growl {

struct GlyphPosition {
	int x;
	int y;
	int w;
	int h;
};

class FontAtlas {
public:
	FontAtlas(
		Font& font, std::unique_ptr<Image> img,
		std::unordered_map<int, GlyphPosition> glyphs)
		: font{font}
		, img{std::move(img)}
		, glyphs{std::move(glyphs)} {}

	const Font& getFont() const {
		return font;
	}

	const Image& getImage() const {
		return *img;
	};

	const std::unordered_map<int, GlyphPosition> getGlyphs() const {
		return glyphs;
	}

private:
	Font& font;
	std::unique_ptr<Image> img;
	std::unordered_map<int, GlyphPosition> glyphs;
};

Result<FontAtlas> createFontAtlasFromFont(Font& font, int size) noexcept;
Result<FontAtlas>
createFontAtlasFromFont(Font& font, int size, std::string text) noexcept;
} // namespace Growl
