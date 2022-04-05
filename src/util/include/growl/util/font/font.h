#pragma once

#include <growl/util/error.h>

namespace Growl {

struct FTFontData;

class Font {
public:
	Font(FTFontData ft_data);
	~Font();

	// Font is move-only
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;
	Font(Font&&) = default;
	Font& operator=(Font&&) = default;

private:
	std::unique_ptr<FTFontData> ft_data;
};

Result<Font> loadFontFromFile(std::string filePath);
} // namespace Growl
