#pragma once

#include "growl/core/assets/image.h"
#include "growl/core/error.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

using nlohmann::json;

namespace Growl {

class Atlas;
class AtlasImagePackInfo {
public:
	friend class Atlas;
	friend Result<Atlas> packAtlasFromFiles(
		std::vector<AtlasImagePackInfo>& images, int padding,
		int bleed_passes) noexcept;

	AtlasImagePackInfo(std::filesystem::path path, int width, int height)
		: path{path}
		, width{width}
		, height{height} {}

private:
	std::filesystem::path path;
	int width;
	int height;
};

struct AtlasRegion {
	float u0;
	float v0;
	float u1;
	float v1;
};

// Turn off name linting as json library needs these.
// NOLINTNEXTLINE(readability-identifier-naming)
void to_json(json& j, const AtlasRegion& r);
// NOLINTNEXTLINE(readability-identifier-naming)
void from_json(const json& j, AtlasRegion& r);

class Atlas {
public:
	Atlas(
		std::unique_ptr<Image> image,
		const std::unordered_map<std::string, AtlasRegion>& mappings)
		: image{std::move(image)}
		, mappings{mappings} {}

	// Atlas is move-only
	Atlas(const Atlas&) = delete;
	Atlas& operator=(const Atlas&) = delete;
	Atlas(Atlas&&) = default;
	Atlas& operator=(Atlas&&) = default;

	Result<AtlasRegion> getRegion(const std::string& name) noexcept;

	const std::unordered_map<std::string, AtlasRegion>& getMappings() const {
		return mappings;
	}

	const Image& getImage() const {
		return *image;
	};

private:
	std::unique_ptr<Image> image;
	std::unordered_map<std::string, AtlasRegion> mappings;
};

Result<Atlas> packAtlasFromFiles(
	std::vector<AtlasImagePackInfo>& images, int padding,
	int bleed_passes) noexcept;

} // namespace Growl
