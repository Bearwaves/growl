#include "../include/growl/util/resources/image.h"
#include "../../contrib/stb_image/stb_image.h"
#include <cassert>

using Growl::Image;

std::unique_ptr<Image> Growl::loadImageFromFile(std::string filePath) {
	int width, height, channels;
	unsigned char* img =
		stbi_load(filePath.c_str(), &width, &height, &channels, 4);
	assert(img != nullptr);
	return std::make_unique<Image>(width, height, channels, img);
}

void Image::StbiDeleter::operator()(unsigned char* data) const {
	stbi_image_free(data);
}
