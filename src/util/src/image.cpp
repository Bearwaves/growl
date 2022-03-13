#include "../include/growl/util/assets/image.h"
#include "../../contrib/stb_image/stb_image.h"

using Growl::BaseError;
using Growl::Image;
using Growl::Result;

struct imageLoadError : BaseError {
	std::string message() override {
		return "Failed to load image from file";
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

void Image::StbiDeleter::operator()(unsigned char* data) const {
	stbi_image_free(data);
}
