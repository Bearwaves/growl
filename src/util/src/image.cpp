#include "../include/growl/util/image/image.h"
#include "../../contrib/stb_image/stb_image.h"
#include <cassert>

using Growl::Image;

Image Growl::loadImageFromFile(std::string filePath) {
	int width, height, channels;
	unsigned char* img =
		stbi_load(filePath.c_str(), &width, &height, &channels, 0);
	assert(img != nullptr);
	return Image{width, height, channels, img};
}

void Image::StbiDeleter::operator()(unsigned char* data) const {
	stbi_image_free(data);
}
