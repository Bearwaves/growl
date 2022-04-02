#pragma once

#include <filesystem>
#include <growl/util/assets/image.h>
#include <growl/util/error.h>
#include <memory>
#include <nlohmann/json.hpp>
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
		std::vector<AtlasImagePackInfo>& images, int padding) noexcept;

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
	int x;
	int y;
	int width;
	int height;
};

void to_json(json& j, const AtlasRegion& r);
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
	std::vector<AtlasImagePackInfo>& images, int padding) noexcept;

} // namespace Growl
