#include "growl/util/assets/font.h"

#include "font_internal.h"
#include "freetype/freetype.h"
#include "freetype/ftmodapi.h"
#include <iostream>
#include <memory>

using Growl::Error;
using Growl::Font;
using Growl::Result;

Font::Font(Growl::FTFontData ft_data)
	: ft_data{std::make_unique<Growl::FTFontData>(std::move(ft_data))} {}

Font::~Font() {
	if (ft_data) {
		FT_Done_Face(ft_data->face);
		FT_Done_FreeType(ft_data->library);
	}
}

Font::Font(Font&& font) = default;
Font& Font::operator=(Font&&) = default;

Result<Font> Growl::loadFontFromFile(std::string filepath) noexcept {
	FT_Library lib;
	FT_Face face;

	if (auto err = FT_Init_FreeType(&lib); err) {
		return Error(
			std::make_unique<FontError>("Failed to init FreeType", err));
	}

	if (auto err = FT_New_Face(lib, filepath.c_str(), 0, &face); err) {
		FT_Done_Library(lib);
		return Error(
			std::make_unique<FontError>("Failed to load font file", err));
	}

	return Font(FTFontData{lib, face});
}
