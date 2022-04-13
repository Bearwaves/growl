#include "growl/util/assets/bundle.h"

#include "growl/util/assets/error.h"
#include "growl/util/assets/image.h"
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

using Growl::AssetInfo;
using Growl::AssetsBundle;
using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::Atlas;
using Growl::Error;
using Growl::Image;
using Growl::Result;

Result<AssetsBundle> Growl::loadAssetsBundle(std::string file_path) noexcept {
	std::ifstream file;
	file.open(file_path, std::ios::binary | std::ios::in);
	if (file.fail()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to open file " + file_path + "; does it exist?"));
	}
	AssetsBundleVersion version;
	file.read(reinterpret_cast<char*>(&version), sizeof(version));
	AssetsBundleMapInfo map_info;
	file.read(reinterpret_cast<char*>(&map_info), sizeof(map_info));
	file.seekg(map_info.position);
	std::string resource_map_json(map_info.size, '\0');
	file.read(resource_map_json.data(), map_info.size);
	AssetsMap resource_map;
	try {
		resource_map = json::parse(resource_map_json);
	} catch (std::exception& e) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load assets map JSON: " + std::string(e.what())));
	}

	return AssetsBundle(file, resource_map);
}

void Growl::to_json(json& j, const AssetInfo& r) {
	j = json{{"position", r.position}, {"size", r.size}, {"typ", r.type}};
	if (r.atlas_regions.has_value()) {
		j["regions"] = r.atlas_regions.value();
	}
}

void Growl::from_json(const json& j, AssetInfo& r) {
	j.at("position").get_to(r.position);
	j.at("size").get_to(r.size);
	j.at("typ").get_to(r.type);
	if (j.contains("regions")) {
		r.atlas_regions =
			j.at("regions").get<std::unordered_map<std::string, AtlasRegion>>();
	}
}

std::string Growl::getAssetTypeName(AssetType type) {
	switch (type) {
	case AssetType::Image:
		return "Image";
	case AssetType::Atlas:
		return "Atlas";
	default:
		return "Unknown";
	}
}

Result<Image> AssetsBundle::getImage(std::string name) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load image " + name + "; not found in asset map."));
	}
	auto& info = info_find->second;

	if (info.type != AssetType::Image) {
		auto type_name = getAssetTypeName(info.type);
		return Error(std::make_unique<AssetsError>(
			"Failed to load image " + name + "; expected Image type but was " +
			type_name + "."));
	}

	std::vector<unsigned char> img_data;
	img_data.reserve(info.size);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(img_data.data()), info.size);

	return loadImageFromMemory(img_data.data(), info.size);
}

Result<Atlas> AssetsBundle::getAtlas(std::string name) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load atlas " + name + "; not found in asset map."));
	}
	auto& info = info_find->second;

	if (info.type != AssetType::Atlas) {
		auto type_name = getAssetTypeName(info.type);
		return Error(std::make_unique<AssetsError>(
			"Failed to load atlas " + name + "; expected Atlas type but was " +
			type_name + "."));
	}

	if (!info.atlas_regions.has_value()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load atlas " + name +
			"; atlas region data is missing."));
	}

	std::vector<unsigned char> img_data;
	img_data.reserve(info.size);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(img_data.data()), info.size);

	Result<Image> image_result =
		loadImageFromMemory(img_data.data(), info.size);
	if (image_result.hasError()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load atlas " + name +
			" image data: " + image_result.error()->message()));
	}

	return Atlas(
		std::make_unique<Image>(std::move(image_result.get())),
		info.atlas_regions.value());
}
