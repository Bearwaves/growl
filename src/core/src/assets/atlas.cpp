#include "growl/core/assets/atlas.h"

#include "growl/core/assets/error.h"
#include "stb_image.h"
#include "stb_rect_pack.h"
#include <algorithm>
#include <cstdint>
#include <cstring>

using Growl::AssetsError;
using Growl::Atlas;
using Growl::AtlasImagePackInfo;
using Growl::AtlasRegion;
using Growl::Error;
using Growl::Result;

constexpr int MAX_SIZE = 8192;

static int nextPowerOfTwo(int n) {
	int i = 2;
	while (i < n) {
		i *= 2;
	}
	return i;
}

struct Pixel {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
};

Result<AtlasRegion> Atlas::getRegion(const std::string& name) noexcept {
	if (auto it = mappings.find(name); it != mappings.end()) {
		return it->second;
	}
	return Error(std::make_unique<AssetsError>(
		"Failed to load atlas region " + name + "; not found in atlas."));
}

Result<Atlas> Growl::packAtlasFromFiles(
	std::vector<AtlasImagePackInfo>& images, int padding,
	int bleed_passes) noexcept {
	stbrp_context ctx;
	std::vector<stbrp_rect> rects;
	int i = 0;
	for (auto& image : images) {
		rects.push_back(stbrp_rect{
			i++, image.width + padding * 2, image.height + padding * 2});
	}
	int width = nextPowerOfTwo(std::max(images[0].width, images[0].height));
	int height = width;
	bool did_pack = false;

	while (width <= MAX_SIZE && height <= MAX_SIZE) {
		std::vector<stbrp_node> nodes(width * 2);
		stbrp_init_target(
			&ctx, width, height, nodes.data(), static_cast<int>(nodes.size()));
		if (stbrp_pack_rects(
				&ctx, rects.data(), static_cast<int>(rects.size()))) {
			did_pack = true;
			break;
		}
		if (width <= height) {
			width *= 2;
		} else {
			height *= 2;
		}
	}

	if (!did_pack) {
		return Error(std::make_unique<AssetsError>(
			"Failed to pack atlas assets within max size texture"));
	}

	std::vector<unsigned char> texture_data(width * height * sizeof(uint32_t));
	std::unordered_map<std::string, AtlasRegion> mappings;
	const float inv_tex_width = 1.0f / width;
	const float inv_tex_height = 1.0f / height;
	for (size_t i = 0; i < rects.size(); i++) {
		auto& rect = rects[i];
		auto& image = images[rect.id];

		int img_width, img_height, img_channels;
		unsigned char* img_data = stbi_load(
			image.path.string().c_str(), &img_width, &img_height, &img_channels,
			4);
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
			(rect.x + padding) * inv_tex_width,
			(rect.y + padding) * inv_tex_height,
			(rect.x + img_width + padding) * inv_tex_width,
			(rect.y + img_height + padding) * inv_tex_height};

		uint32_t* img_32 = reinterpret_cast<uint32_t*>(img_data);

		for (int j = 0; j < img_height; j++) {
			auto dst = texture_data.data() +
					   sizeof(uint32_t) *
						   ((rect.y + padding + j) * width + rect.x + padding);
			std::memcpy(
				dst, img_32 + (j * img_width), img_width * sizeof(uint32_t));
		}
	}

	for (int pass = 0; pass < bleed_passes; pass++) {
		std::vector<unsigned char> previous_data = texture_data;
		Pixel* old_pixels = reinterpret_cast<Pixel*>(previous_data.data());
		Pixel* new_pixel = reinterpret_cast<Pixel*>(texture_data.data());
		for (int i = 0; i < texture_data.size() / 4; i++) {
			if (*reinterpret_cast<uint32_t*>(new_pixel) == 0) {
				uint32_t r = 0, g = 0, b = 0;
				uint8_t sum = 0;
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1; y++) {
						int index = i + (y * width) + x;
						if (index == i || index < 0 ||
							index > texture_data.size() / 4) {
							continue;
						}
						Pixel* p = old_pixels + index;
						if (!(p->r || p->g || p->b)) {
							continue;
						}
						r += p->r;
						g += p->g;
						b += p->b;
						sum++;
					}
				}
				if (sum) {
					new_pixel->r = r / sum;
					new_pixel->g = g / sum;
					new_pixel->b = b / sum;
				}
			}
			new_pixel++;
		}
	}

	return Atlas(
		std::make_unique<Image>(width, height, 4, texture_data), mappings);
}

void Growl::to_json(json& j, const AtlasRegion& r) {
	j = json{{"u0", r.u0}, {"v0", r.v0}, {"u1", r.u1}, {"v1", r.v1}};
}

void Growl::from_json(const json& j, AtlasRegion& r) {
	j.at("u0").get_to(r.u0);
	j.at("v0").get_to(r.v0);
	j.at("u1").get_to(r.u1);
	j.at("v1").get_to(r.v1);
}
