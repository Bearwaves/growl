#pragma once

#include "growl/util/error.h"
#include <vector>

namespace Growl {

struct FTFontData;

class Font {
public:
	explicit Font(FTFontData ft_data);
	Font(FTFontData ft_data, std::vector<unsigned char>&& data);
	~Font();

	// Font is move-only
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;
	Font(Font&&);
	Font& operator=(Font&&);

	const FTFontData& getFTFontData() const {
		return *ft_data;
	}

private:
	std::unique_ptr<FTFontData> ft_data;
	std::vector<unsigned char> data;
};

Result<Font> loadFontFromFile(std::string file_path) noexcept;
Result<Font> loadFontFromMemory(std::vector<unsigned char>&& data) noexcept;
} // namespace Growl
