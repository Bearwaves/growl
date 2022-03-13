#include "../include/growl/util/assets/bundle.h"
#include "../include/growl/util/assets/error.h"
#include "growl/util/assets/image.h"
#include <cstdint>
#include <stdexcept>
#include <vector>

using Growl::AssetInfo;
using Growl::AssetsBundle;
using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::Error;
using Growl::Image;
using Growl::Result;

Result<AssetsBundle> Growl::loadAssetsBundle(std::string filePath) noexcept {
	std::ifstream file;
	file.open(filePath, std::ios::binary | std::ios::in);
	AssetsBundleVersion version;
	file.read(reinterpret_cast<char*>(&version), sizeof(version));
	AssetsBundleMapInfo mapInfo;
	file.read(reinterpret_cast<char*>(&mapInfo), sizeof(mapInfo));
	file.seekg(mapInfo.position);
	std::vector<char> resourceMapJson;
	resourceMapJson.reserve(mapInfo.size);
	file.read(resourceMapJson.data(), mapInfo.size);
	AssetsMap resourceMap;
	try {
		resourceMap = json::parse(std::string(resourceMapJson.data()));
	} catch (std::exception& e) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load assets map JSON: " + std::string(e.what())));
	}

	return AssetsBundle(file, resourceMap);
}

void Growl::to_json(json& j, const AssetInfo& r) {
	j = json{{"position", r.position}, {"size", r.size}, {"typ", r.type}};
}

void Growl::from_json(const json& j, AssetInfo& r) {
	j.at("position").get_to(r.position);
	j.at("size").get_to(r.size);
	j.at("typ").get_to(r.type);
}

std::string Growl::getAssetTypeName(AssetType type) {
	switch (type) {
	case AssetType::Image:
		return "Image";
	default:
		return "Unknown";
	}
}

Result<Image> AssetsBundle::getImage(std::string name) noexcept {
	AssetInfo info;
	try {
		info = assetsMap.at(name);
	} catch (std::out_of_range&) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load asset " + name + "; not found in asset map."));
	}

	if (info.type != AssetType::Image) {
		auto typeName = getAssetTypeName(info.type);
		return Error(std::make_unique<AssetsError>(
			"Failed to load asset " + name + "; expected Image type but was " +
			typeName + "."));
	}

	std::vector<unsigned char> imgData;
	imgData.reserve(info.size);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(imgData.data()), info.size);

	return loadImageFromMemory(imgData.data(), info.size);
}
