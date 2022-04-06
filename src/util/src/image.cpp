#include <growl/util/assets/image.h>

#include "../../../thirdparty/fpng/fpng.h"
#include "../../../thirdparty/stb_image/stb_image.h"
#include <memory>

using Growl::BaseError;
using Growl::Image;
using Growl::Result;

struct imageLoadError : BaseError {
	std::string message() override {
		return "Failed to load image";
	}
};

Result<Image> Growl::loadImageFromFile(std::string filePath) {
	int width, height, channels;
	unsigned char* img =
		stbi_load(filePath.c_str(), &width, &height, &channels, 4);
	if (img == nullptr) {
		return Error(std::make_unique<imageLoadError>());
	}
	return Image(width, height, channels, img);
}

Result<Image>
Growl::loadImageFromMemory(const unsigned char* address, uint64_t size) {
	fpng::fpng_init();
	uint32_t width, height, channels;
	std::vector<unsigned char> data;
	if (fpng::fpng_decode_memory(
			address, size, data, width, height, channels, 4) !=
		fpng::FPNG_DECODE_SUCCESS) {
		return Error(std::make_unique<imageLoadError>());
	}
	return Image(width, height, channels, data);
}

void Image::StbiDeleter::operator()(unsigned char* data) const {
	stbi_image_free(data);
}
