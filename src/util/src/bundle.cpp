#include "../include/growl/util/assets/bundle.h"
#include "../include/growl/util/assets/error.h"
#include <cstdint>

using Growl::AssetInfo;
using Growl::AssetsBundle;
using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::Error;
using Growl::Result;

Result<AssetsBundle> Growl::loadAssetsBundle(std::string filePath) noexcept {
	std::ifstream file;
	file.open(filePath, std::ios::binary | std::ios::in);
	AssetsBundleVersion version;
	file.read(reinterpret_cast<char*>(&version), sizeof(version));
	AssetsBundleMapInfo mapInfo;
	file.read(reinterpret_cast<char*>(&mapInfo), sizeof(mapInfo));
	file.seekg(mapInfo.position);
	char resourceMapJson[mapInfo.size];
	file.read(reinterpret_cast<char*>(&resourceMapJson), mapInfo.size);
	AssetsMap resourceMap;
	try {
		resourceMap = json::parse(std::string(resourceMapJson));
	} catch (std::exception& e) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load assets map JSON: " + std::string(e.what())));
	}

	return AssetsBundle(file, resourceMap);
}

void Growl::to_json(json& j, const AssetInfo& r) {
	j = json{{"position", r.position}, {"size", r.size}};
}

void Growl::from_json(const json& j, AssetInfo& r) {
	j.at("position").get_to(r.position);
	j.at("size").get_to(r.size);
}
