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
	friend Result<Atlas>
	packAtlasFromFiles(std::vector<AtlasImagePackInfo> images) noexcept;

	AtlasImagePackInfo(std::filesystem::path path, int width, int height)
		: path{path}
		, width{width}
		, height{height} {}

private:
	std::filesystem::path path;
	int width;
	int height;
	int x;
	int y;
};

struct AtlasRegion {
	std::string name;
	int x;
	int y;
	int width;
	int height;
};

void to_json(json& j, const AtlasRegion& r);
void from_json(const json& j, AtlasRegion& r);

class Atlas {
public:
	Atlas(std::unique_ptr<Image> image, std::vector<AtlasImagePackInfo> pack);
	Result<AtlasRegion> getRegion(std::string name) noexcept;
	std::vector<AtlasRegion> getRegions() noexcept;
	Image* getImage() {
		return image.get();
	};

private:
	std::unique_ptr<Image> image;
	std::unordered_map<std::string, AtlasRegion> mappings;
};

Result<Atlas>
packAtlasFromFiles(std::vector<AtlasImagePackInfo> images) noexcept;

} // namespace Growl
