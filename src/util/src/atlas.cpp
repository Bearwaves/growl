#include <growl/util/assets/atlas.h>

#include "../../../thirdparty/stb_image/stb_image.h"
#include "../../../thirdparty/stb_rect_pack/stb_rect_pack.h"
#include <algorithm>
#include <cstring>
#include <growl/util/assets/error.h>

using Growl::AssetsError;
using Growl::Atlas;
using Growl::AtlasImagePackInfo;
using Growl::AtlasRegion;
using Growl::Error;
using Growl::Result;

constexpr int MAX_SIZE = 8192;

Result<AtlasRegion> Atlas::getRegion(const std::string& name) noexcept {
	if (auto it = mappings.find(name); it != mappings.end()) {
		return it->second;
	}
	return Error(std::make_unique<AssetsError>(
		"Failed to load atlas region " + name + "; not found in atlas."));
}

int nextPowerOfTwo(int n) {
	int i = 2;
	while (i < n) {
		i *= 2;
	}
	return i;
}

Result<Atlas> Growl::packAtlasFromFiles(
	std::vector<AtlasImagePackInfo>& images, int padding) noexcept {
	stbrp_context ctx;
	std::vector<stbrp_rect> rects;
	int i = 0;
	for (auto& image : images) {
		rects.push_back(stbrp_rect{
			i++, image.width + padding * 2, image.height + padding * 2});
	}
	int width = nextPowerOfTwo(std::max(images[0].width, images[0].height));
	int height = width;
	bool resize_width_next = true;
	bool did_pack = false;

	while (width <= MAX_SIZE && height <= MAX_SIZE) {
		std::vector<stbrp_node> nodes(width * 2);
		stbrp_init_target(&ctx, width, height, nodes.data(), nodes.size());
		if (stbrp_pack_rects(&ctx, rects.data(), rects.size())) {
			did_pack = true;
			break;
		}
		if (resize_width_next) {
			width *= 2;
		} else {
			height *= 2;
		}
		resize_width_next = !resize_width_next;
	}

	if (!did_pack) {
		return Error(std::make_unique<AssetsError>(
			"Failed to pack atlas assets within max size texture"));
	}

	std::vector<unsigned char> texture_data(width * height * sizeof(uint32_t));
	std::unordered_map<std::string, AtlasRegion> mappings;
	for (size_t i = 0; i < rects.size(); i++) {
		auto& rect = rects[i];
		auto& image = images[rect.id];

		int img_width, img_height, img_channels;
		unsigned char* img_data = stbi_load(
			image.path.c_str(), &img_width, &img_height, &img_channels, 4);
		if (!img_data) {
			return Error(std::make_unique<AssetsError>(
				"Failed to load atlas image data"));
		}
		Image img(img_width, img_height, img_channels, img_data);
		if (img_width != rect.w - padding * 2 ||
			img_height != rect.h - padding * 2) {
			return Error(std::make_unique<AssetsError>(
				"Failed to parse image data: wrong dimensions"));
		}

		mappings[image.path.filename().string()] = AtlasRegion{
			rect.x + padding, rect.y + padding, img_width, img_height};

		uint32_t* img_32 = reinterpret_cast<uint32_t*>(img_data);

		// Dilation passes
		// TODO improve performance and correctness
		for (int py = 0; py <= padding * 2; py++) {
			for (int px = 0; px <= padding * 2; px++) {
				if (px == padding && py == padding) {
					// Do centre draw after
					continue;
				}
				for (int j = 0; j < img_height; j++) {
					auto dst = texture_data.data() +
							   sizeof(uint32_t) *
								   ((rect.y + py + j) * width + rect.x + px);
					std::memcpy(
						dst, img_32 + (j * img_width),
						img_width * sizeof(uint32_t));
				}
			}
		}

		for (int j = 0; j < img_height; j++) {
			auto dst = texture_data.data() +
					   sizeof(uint32_t) *
						   ((rect.y + padding + j) * width + rect.x + padding);
			std::memcpy(
				dst, img_32 + (j * img_width), img_width * sizeof(uint32_t));
		}
	}

	return Atlas(
		std::make_unique<Image>(width, height, 4, texture_data), mappings);
}

void Growl::to_json(json& j, const AtlasRegion& r) {
	j = json{{"x", r.x}, {"y", r.y}, {"width", r.width}, {"height", r.height}};
}

void Growl::from_json(const json& j, AtlasRegion& r) {
	j.at("x").get_to(r.x);
	j.at("y").get_to(r.y);
	j.at("width").get_to(r.width);
	j.at("height").get_to(r.height);
}
