#include "growl/core/assets/image.h"

#include "stb_image.h"
#include <memory>

using Growl::BaseError;
using Growl::Image;
using Growl::Result;

namespace {
struct ImageLoadError : BaseError {
	std::string message() override {
		return "Failed to load image";
	}
};
} // namespace

Result<Image> Growl::loadImageFromFile(std::string file_path) {
	int width, height, channels;
	unsigned char* img =
		stbi_load(file_path.c_str(), &width, &height, &channels, 4);
	if (img == nullptr) {
		return Error(std::make_unique<ImageLoadError>());
	}
	return Image(width, height, channels, img);
}

Result<Image>
Growl::loadImageFromMemory(const unsigned char* address, uint64_t size) {
	int width, height, channels;
	std::vector<unsigned char> data;
	unsigned char* img =
		stbi_load_from_memory(address, size, &width, &height, &channels, 4);
	if (img == nullptr) {
		return Error(std::make_unique<ImageLoadError>());
	}
	return Image(width, height, channels, img);
}

void Image::StbiDeleter::operator()(unsigned char* data) const {
	stbi_image_free(data);
}
