#pragma once

#include "../error.h"
#include <stdint.h>
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
		, data{std::move(data)} {}

	~Image() = default;

	// Image is move-only
	Image(const Image&) = delete;
	Image& operator=(const Image&) = delete;
	Image(Image&&) = default;
	Image& operator=(Image&&) = default;

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

Result<Image> loadImageFromFile(std::string file_path);
Result<Image> loadImageFromMemory(const unsigned char* address, uint64_t size);
} // namespace Growl
