#include "../include/growl/util/assets/atlas.h"
#include "../../contrib/stb_image/stb_image.h"
#include "../../contrib/stb_rect_pack/stb_rect_pack.h"
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

Atlas::Atlas(
	std::unique_ptr<Image> image, std::vector<AtlasImagePackInfo> pack) {
	this->image = std::move(image);
	for (const auto& info : pack) {
		std::string name = info.path.filename().string();
		mappings.emplace(
			name,
			AtlasRegion{name, info.x, info.y, info.width, info.height});
	}
}

Result<AtlasRegion> Atlas::getRegion(std::string name) noexcept {
	try {
		return mappings.at(name);
	} catch (std::out_of_range&) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load atlas region " + name + "; not found in atlas."));
	}
}

std::vector<AtlasRegion> Atlas::getRegions() noexcept {
	std::vector<AtlasRegion> regions;
	for (auto [_, region] : mappings) {
		regions.push_back(region);
	}
	return regions;
}

int nextPowerOfTwo(int n) {
	int i = 2;
	while (i < n) {
		i *= 2;
	}
	return i;
}

Result<Atlas>
Growl::packAtlasFromFiles(std::vector<AtlasImagePackInfo> images) noexcept {
	stbrp_context ctx;
	std::vector<stbrp_rect> rects;
	int i = 0;
	for (auto& image : images) {
		rects.push_back(stbrp_rect{i++, image.width, image.height});
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
		if (img_width != rect.w || img_height != rect.h) {
			return Error(std::make_unique<AssetsError>(
				"Failed to parse image data: wrong dimensions"));
		}

		image.width = rect.w;
		image.height = rect.h;
		image.x = rect.x;
		image.y = rect.y;

		uint32_t* img_32 = reinterpret_cast<uint32_t*>(img_data);

		for (int j = 0; j < img_height; j++) {
			auto dst = texture_data.data() +
					   sizeof(uint32_t) * ((rect.y + j) * width + rect.x);
			std::memcpy(
				dst, img_32 + (j * img_width), img_width * sizeof(uint32_t));
		}
	}

	return Atlas(
		std::make_unique<Image>(width, height, 4, texture_data), images);
}

void Growl::to_json(json& j, const AtlasRegion& r) {
	j = json{
		{"x", r.x},
		{"y", r.y},
		{"width", r.width},
		{"height", r.height},
		{"name", r.name}};
}

void Growl::from_json(const json& j, AtlasRegion& r) {
	j.at("x").get_to(r.x);
	j.at("y").get_to(r.y);
	j.at("width").get_to(r.width);
	j.at("height").get_to(r.height);
	j.at("name").get_to(r.name);
}
