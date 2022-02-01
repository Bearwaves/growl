#include "../include/growl/util/image/image.h"
#include "../../contrib/stb_image/stb_image.h"
#include <cassert>

using Growl::Image;

Image::~Image() {
	stbi_image_free(raw);
}

Image Growl::loadImageFromFile(std::string filePath) {
	int width, height, channels;
	unsigned char* img =
		stbi_load(filePath.c_str(), &width, &height, &channels, 0);
	assert(img != nullptr);
	return Image{width, height, channels, img};
}
