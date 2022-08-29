#include "growl/core/assets/bundle.h"

#include "growl/core/assets/error.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"
#include <cstdint>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

using Growl::AssetInfo;
using Growl::AssetsBundle;
using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::Atlas;
using Growl::Error;
using Growl::FontFace;
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

	return AssetsBundle(file, file_path, resource_map);
}

void Growl::to_json(json& j, const AssetInfo& r) {
	j = json{{"position", r.position}, {"size", r.size}, {"typ", r.type}};
	if (r.atlas_regions.has_value()) {
		j["regions"] = r.atlas_regions.value();
	}
	if (r.font.has_value()) {
		j["font"] = r.font.value();
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
	if (j.contains("font")) {
		r.font = j.at("font").get<AssetsBundleMSDFFontInfo>();
	}
}

void Growl::to_json(json& j, const AssetsBundleMSDFFontInfo& r) {
	j = json{
		{"msdfPosition", r.msdf_position},
		{"msdfSize", r.msdf_size},
		{"glyphs", r.glyphs}};
}

void Growl::from_json(const json& j, AssetsBundleMSDFFontInfo& r) {
	j.at("msdfPosition").get_to(r.msdf_position);
	j.at("msdfSize").get_to(r.msdf_size);
	j.at("glyphs").get_to(r.glyphs);
}

std::string Growl::getAssetTypeName(AssetType type) {
	switch (type) {
	case AssetType::Image:
		return "Image";
	case AssetType::Atlas:
		return "Atlas";
	case AssetType::Font:
		return "Font";
	case AssetType::Audio:
		return "Sound";
	case AssetType::Text:
		return "Text";
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

Result<FontFace> AssetsBundle::getBitmapFont(
	std::string name, int size, std::string characters) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load font " + name + "; not found in asset map."));
	}
	auto& info = info_find->second;

	if (info.type != AssetType::Font) {
		auto type_name = getAssetTypeName(info.type);
		return Error(std::make_unique<AssetsError>(
			"Failed to load font " + name + "; expected Font type but was " +
			type_name + "."));
	}

	std::vector<unsigned char> font_data;
	font_data.resize(info.size);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(font_data.data()), info.size);

	return createBitmapFontFaceFromMemory(
		std::move(font_data), size, characters);
}

Result<FontFace> AssetsBundle::getDistanceFieldFont(std::string name) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load font " + name + "; not found in asset map."));
	}
	auto& info = info_find->second;

	if (info.type != AssetType::Font) {
		auto type_name = getAssetTypeName(info.type);
		return Error(std::make_unique<AssetsError>(
			"Failed to load font " + name + "; expected Font type but was " +
			type_name + "."));
	}

	std::vector<unsigned char> font_data;
	font_data.resize(info.size);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(font_data.data()), info.size);

	if (!info.font.has_value()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load MSDF data from font"));
	}

	std::vector<unsigned char> image_data;
	image_data.resize(info.font.value().msdf_size);
	file.seekg(info.font.value().msdf_position);
	file.read(
		reinterpret_cast<char*>(image_data.data()),
		info.font.value().msdf_size);

	Result<Image> image_result =
		loadImageFromMemory(image_data.data(), info.font.value().msdf_size);
	if (image_result.hasError()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load MSDF image: " + image_result.error()->message()));
	}

	return createDistanceFieldFontFaceFromBundleData(
		std::move(font_data),
		std::make_unique<Image>(std::move(image_result.get())),
		std::move(info.font.value().glyphs));
}

Result<std::string>
AssetsBundle::getTextFileAsString(std::string name) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load text file " + name + "; not found in asset map."));
	}
	auto& info = info_find->second;

	if (info.type != AssetType::Text) {
		auto type_name = getAssetTypeName(info.type);
		return Error(std::make_unique<AssetsError>(
			"Failed to load text file " + name +
			"; expected Text type but was " + type_name + "."));
	}

	std::string data(info.size, 0);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(data.data()), info.size);

	return std::move(data);
}

Result<std::vector<unsigned char>>
AssetsBundle::getRawData(std::string name) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load asset " + name + "; not found in asset map."));
	}
	auto& info = info_find->second;

	std::vector<unsigned char> data;
	data.resize(info.size);
	file.seekg(info.position);
	file.read(reinterpret_cast<char*>(data.data()), info.size);

	return std::move(data);
}

Result<std::ifstream> AssetsBundle::openNewStream() noexcept {
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);
	if (file.fail()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to open file " + path + "; has it moved?"));
	}
	return std::move(file);
}
