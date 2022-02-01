#pragma once

#include <cstddef>
#include <string>

namespace Growl {

class Image {
public:
	explicit Image(int width, int height, int channels, unsigned char* raw)
		: width{width}
		, height{height}
		, channels{channels}
		, raw{raw} {}
	~Image();
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&&) = delete;
	Image& operator=(Image&&) = delete;

	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	int getChannels() const {
		return channels;
	}

private:
	int width;
	int height;
	int channels;
	unsigned char* raw;
};

Image loadImageFromFile(std::string filePath);
} // namespace Growl
