#pragma once

#include <cstddef>
#include <iostream>
#include <string>

namespace Growl {

class Image {

public:
	Image(int width, int height, int channels, unsigned char* raw)
		: width{width}
		, height{height}
		, channels{channels}
		, raw{StbiImage{raw}} {}

	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	int getChannels() const {
		return channels;
	}
	unsigned char* getRaw() const {
		return raw.get();
	}
	bool useFiltering() const {
		return true;
	}

private:
	int width;
	int height;
	int channels;

	class StbiDeleter {
	public:
		void operator()(unsigned char* data) const;
	};
	using StbiImage = std::unique_ptr<unsigned char[], StbiDeleter>;
	StbiImage raw;
};

std::unique_ptr<Image> loadImageFromFile(std::string filePath);
} // namespace Growl
