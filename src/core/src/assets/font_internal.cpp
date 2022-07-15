#include "font_internal.h"
#include "growl/core/error.h"

using Growl::Error;
using Growl::FTFontData;
using Growl::Result;

constexpr int MAX_SIZE = 8192;

Error Growl::Internal::setFontFacePixelSize(
	FTFontData& font_data, int size) noexcept {
	if (font_data.face->num_fixed_sizes) {
		int best = 0;
		int diff = std::abs(size - font_data.face->available_sizes[0].height);
		for (int i = 1; i < font_data.face->num_fixed_sizes; i++) {
			int ndiff =
				std::abs(size - font_data.face->available_sizes[i].height);
			if (ndiff < diff) {
				diff = ndiff;
				best = i;
			}
		}
		if (auto err = FT_Select_Size(font_data.face, best); err) {
			return Error(std::make_unique<FontError>(
				"Failed to select font face size", err));
		}
		return nullptr;
	}

	if (auto err = FT_Set_Pixel_Sizes(font_data.face, 0, size); err) {
		return Error(
			std::make_unique<FontError>("Failed to set font face size", err));
	}
	return nullptr;
}

bool Growl::Internal::packRectsIncreasing(
	std::vector<stbrp_rect>& rects, int start_size, int* out_width,
	int* out_height) {
	int width = start_size;
	int height = width;
	while (width <= MAX_SIZE && height <= MAX_SIZE) {
		stbrp_context ctx;
		std::vector<stbrp_node> nodes(width * 2);
		stbrp_init_target(
			&ctx, width, height, nodes.data(), static_cast<int>(nodes.size()));
		if (stbrp_pack_rects(
				&ctx, rects.data(), static_cast<int>(rects.size()))) {
			*out_width = width;
			*out_height = height;
			return true;
		}
		if (width <= height) {
			width *= 2;
		} else {
			height *= 2;
		}
	}
	return false;
}
