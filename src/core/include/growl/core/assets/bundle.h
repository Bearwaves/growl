#pragma once

#include "atlas.h"
#include "font_face.h"
#include "growl/core/error.h"
#include "image.h"
#include "nlohmann/json.hpp"
#include <cstdint>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>

using nlohmann::json;

namespace Growl {

typedef uint64_t AssetsBundleVersion;

constexpr AssetsBundleVersion ASSETS_BUNDLE_LATEST_VERSION = 1;

enum class AssetType : uint64_t { Unknown, Image, Atlas, Font, Audio };

std::string getAssetTypeName(AssetType type);

struct AssetsBundleMapInfo {
	uint64_t position;
	uint64_t size;
};

struct AssetsBundleMSDFFontInfo {
	uint64_t msdf_position;
	uint64_t msdf_size;
	std::unordered_map<int, AtlasRegion> glyphs;
};

struct AssetInfo {
	uint64_t position;
	uint64_t size;
	AssetType type;
	std::optional<std::unordered_map<std::string, AtlasRegion>> atlas_regions;
	std::optional<AssetsBundleMSDFFontInfo> font;
};

// Turn off name linting as json library needs these.
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AssetInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AssetsBundleMSDFFontInfo& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AssetsBundleMSDFFontInfo& r);

using AssetsMap = std::map<std::string, AssetInfo>;

class AssetsBundle {
public:
	explicit AssetsBundle(std::ifstream& file, AssetsMap& assets_map) noexcept
		: file{std::move(file)}
		, assetsMap{std::move(assets_map)} {}

	AssetsMap& getAssetsMap() {
		return assetsMap;
	}

	Result<Image> getImage(std::string name) noexcept;
	Result<Atlas> getAtlas(std::string name) noexcept;
	Result<FontFace> getBitmapFont(
		std::string name, int size, std::string characters = "") noexcept;
	Result<FontFace> getDistanceFieldFont(std::string name) noexcept;

private:
	std::ifstream file;
	AssetsMap assetsMap;
};

Result<AssetsBundle> loadAssetsBundle(std::string file_path) noexcept;

} // namespace Growl
