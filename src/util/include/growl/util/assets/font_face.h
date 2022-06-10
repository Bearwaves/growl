#pragma once

#include "atlas.h"
#include "growl/util/error.h"
#include "image.h"
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace Growl {

enum class FontFaceType { Bitmap, SDF, MSDF };

class FTFontData;

class FontFace {
public:
	FontFace(
		FontFaceType type, FTFontData font_data, std::unique_ptr<Image> img,
		std::unordered_map<int, AtlasRegion> glyphs);
	~FontFace();

	// FontFace is move-only
	FontFace(const FontFace&) = delete;
	FontFace& operator=(const FontFace&) = delete;
	FontFace(FontFace&&);
	FontFace& operator=(FontFace&&);

	// setSize sets the size, in pixels, of the font. This is an expensive
	// operation for Bitmap fonts.
	void setSize(int size);

	const FontFaceType getType() const {
		return type;
	}

	FTFontData& getFontData() const {
		return *font_data;
	}

	const Image& getImage() const {
		return *img;
	};

	const std::unordered_map<int, AtlasRegion> getGlyphs() const {
		return glyphs;
	}

private:
	FontFaceType type;
	std::unique_ptr<FTFontData> font_data;
	std::unique_ptr<Image> img;
	std::unordered_map<int, AtlasRegion> glyphs;
};

bool isValidFont(std::string path);
Result<FontFace> createBitmapFontFaceFromMemory(
	std::vector<unsigned char>&& data, int size, std::string characters = "");
Result<FontFace> createBitmapFontFaceFromFile(
	std::string path, int size, std::string characters = "");
Result<FontFace> createDistanceFieldFontFaceFromMemory(
	std::vector<unsigned char>&& data, int size, std::string characters = "");
Result<FontFace> createDistanceFieldFontFaceFromFile(
	std::string path, int size, std::string characters = "");
Result<FontFace> createDistanceFieldFontFaceFromBundleData(
	std::vector<unsigned char>&& font_data, std::unique_ptr<Image> img,
	std::unordered_map<int, AtlasRegion>&& glyphs);

} // namespace Growl
