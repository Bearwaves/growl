#include "growl/core/assets/font_face.h"
#include "font_internal.h"
#include "freetype/freetype.h"
#include "freetype/ftmodapi.h"
#include "growl/core/assets/atlas.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"

using Growl::AtlasRegion;
using Growl::Error;
using Growl::FontFace;
using Growl::FTFontData;
using Growl::Image;
using Growl::Result;

Result<FTFontData> loadFont(std::string path) noexcept {
	FT_Library lib;
	FT_Face face;

	if (auto err = FT_Init_FreeType(&lib)) {
		return Error(
			std::make_unique<FontError>("Failed to init FreeType", err));
	}

	if (auto err = FT_New_Face(lib, path.c_str(), 0, &face)) {
		FT_Done_Library(lib);
		return Error(
			std::make_unique<FontError>("Failed to load font file", err));
	}

	return FTFontData{lib, face};
}

Result<FTFontData> loadFont(std::vector<unsigned char>&& data) noexcept {
	FT_Library lib;
	FT_Face face;

	if (auto err = FT_Init_FreeType(&lib)) {
		return Error(
			std::make_unique<FontError>("Failed to init FreeType", err));
	}

	if (auto err = FT_New_Memory_Face(lib, data.data(), data.size(), 0, &face);
		err) {
		FT_Done_Library(lib);
		return Error(
			std::make_unique<FontError>(
				"Failed to load font from memory", err));
	}

	return FTFontData{lib, face, std::move(data)};
}

FontFace::FontFace(
	FontFaceType type, FTFontData font_data, std::unique_ptr<Image> img,
	std::unordered_map<int, AtlasRegion> glyphs, float pixel_range)
	: type{type}
	, font_data{std::make_unique<FTFontData>(std::move(font_data))}
	, img{std::move(img)}
	, glyphs{std::move(glyphs)}
	, pixel_range{pixel_range} {}

FontFace::~FontFace() = default;
FontFace::FontFace(FontFace&&) = default;
FontFace& FontFace::operator=(FontFace&&) = default;

// Creation functions

Result<FontFace>
createBitmapFontFace(FTFontData data, int size, std::string characters);
Result<FontFace>
createDistanceFieldFontFace(FTFontData data, int size, std::string characters);

bool Growl::isValidFont(std::string path) {
	return !loadFont(path).hasError();
}

Result<FontFace> Growl::createBitmapFontFaceFromMemory(
	std::vector<unsigned char>&& data, int size, std::string characters) {
	auto data_result = loadFont(std::move(data));
	if (data_result.hasError()) {
		return std::move(data_result.error());
	}
	return createBitmapFontFace(std::move(data_result.get()), size, characters);
}

Result<FontFace> Growl::createBitmapFontFaceFromFile(
	std::string path, int size, std::string characters) {
	auto data_result = loadFont(path);
	if (data_result.hasError()) {
		return std::move(data_result.error());
	}
	return createBitmapFontFace(std::move(data_result.get()), size, characters);
}

Result<FontFace> Growl::createDistanceFieldFontFaceFromMemory(
	std::vector<unsigned char>&& data, int size, std::string characters) {
	auto data_result = loadFont(std::move(data));
	if (data_result.hasError()) {
		return std::move(data_result.error());
	}
	return createDistanceFieldFontFace(
		std::move(data_result.get()), size, characters);
}

Result<FontFace> Growl::createDistanceFieldFontFaceFromFile(
	std::string path, int size, std::string characters) {
	auto data_result = loadFont(path);
	if (data_result.hasError()) {
		return std::move(data_result.error());
	}
	return createDistanceFieldFontFace(
		std::move(data_result.get()), size, characters);
}

Result<FontFace> Growl::createDistanceFieldFontFaceFromBundleData(
	std::vector<unsigned char>&& font_data, std::unique_ptr<Image> img,
	std::unordered_map<int, AtlasRegion>&& glyphs, float pixel_range) {
	auto data_result = loadFont(std::move(font_data));
	if (data_result.hasError()) {
		return std::move(data_result.error());
	}
	return FontFace(
		FontFaceType::MSDF, std::move(data_result.get()), std::move(img),
		std::move(glyphs), pixel_range);
}
