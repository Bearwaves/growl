#pragma once

#include "image.h"
#include <cstdint>
#include <fstream>
#include <growl/util/error.h>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

using nlohmann::json;

namespace Growl {

typedef uint64_t AssetsBundleVersion;

constexpr AssetsBundleVersion ASSETS_BUNDLE_LATEST_VERSION = 1;

enum class AssetType : uint64_t { Unknown, Image };

std::string getAssetTypeName(AssetType type);

struct AssetsBundleMapInfo {
	uint64_t position;
	uint64_t size;
};

struct AssetInfo {
	uint64_t position;
	uint64_t size;
	AssetType type;
};

void to_json(json& j, const AssetInfo& r);
void from_json(const json& j, AssetInfo& r);

using AssetsMap = std::map<std::string, AssetInfo>;

class AssetsBundle {
public:
	explicit AssetsBundle(std::ifstream& file, AssetsMap& assetsMap) noexcept
		: file{std::move(file)}
		, assetsMap{std::move(assetsMap)} {}

	AssetsMap& getAssetsMap() {
		return assetsMap;
	}

	Result<Image> getImage(std::string name) noexcept;

private:
	std::ifstream file;
	AssetsMap assetsMap;
};

Result<AssetsBundle> loadAssetsBundle(std::string filePath) noexcept;

} // namespace Growl
