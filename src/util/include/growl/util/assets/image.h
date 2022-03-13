#pragma once

#include "../error.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace Growl {

class Image {

public:
	Image(int width, int height, int channels, unsigned char* raw)
		: width{width}
		, height{height}
		, channels{channels}
		, raw{StbiImage{raw}} {}

	Image(int width, int height, int channels, std::vector<unsigned char> data)
		: width{width}
		, height{height}
		, channels{channels}
		, data{data} {}

	int getWidth() const {
		return width;
	}
	int getHeight() const {
		return height;
	}
	int getChannels() const {
		return channels;
	}
	const unsigned char* getRaw() const {
		if (raw) {
			return raw.get();
		}
		return data.data();
	}
	bool useFiltering() const {
		return true;
	}

private:
	int width;
	int height;
	int channels;
	std::vector<unsigned char> data;

	class StbiDeleter {
	public:
		void operator()(unsigned char* data) const;
	};
	using StbiImage = std::unique_ptr<unsigned char[], StbiDeleter>;
	StbiImage raw;
};

Result<Image> loadImageFromFile(std::string filePath);
Result<Image> loadImageFromMemory(const unsigned char* address, uint64_t size);
} // namespace Growl
