#pragma once

#include "font.h"
#include "image.h"
#include <memory>

namespace Growl {

class FontAtlas {
public:
	FontAtlas(Font& font, std::unique_ptr<Image> img)
		: font{font}
		, img{std::move(img)} {}

private:
	Font& font;
	std::unique_ptr<Image> img;
};

Result<FontAtlas>
createFontAtlasFromFont(Font& font, int size, std::string text) noexcept;
} // namespace Growl
