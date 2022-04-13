#include "growl/core/graphics/font_texture_atlas.h"
#include "growl/util/assets/error.h"

using Growl::FontTextureAtlas;
using Growl::GlyphPosition;
using Growl::Result;

Result<GlyphPosition> FontTextureAtlas::getRegion(int glyph_code) const {
	if (auto it = glyphs.find(glyph_code); it != glyphs.end()) {
		return it->second;
	}
	return Error(std::make_unique<AssetsError>(
		"Failed to find glyph " + std::to_string(glyph_code) + " in atlas."));
}
