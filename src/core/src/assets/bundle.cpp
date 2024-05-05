#include "growl/core/assets/bundle.h"

#include "growl/core/api/system_api.h"
#include "growl/core/assets/atlas.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/image.h"
#include "growl/core/error.h"
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
using Growl::File;
using Growl::FontFace;
using Growl::Image;
using Growl::Result;
using Growl::SystemAPI;

Result<AssetsBundle>
Growl::loadAssetsBundle(SystemAPI& system, std::string file_path) noexcept {
	auto file_result = system.openFile(file_path);
	if (file_result.hasError()) {
		return std::move(file_result.error());
	}
	std::unique_ptr<File> file = std::move(file_result.get());
	return loadAssetsBundle(std::move(file), file_path);
}

Result<AssetsBundle> Growl::loadAssetsBundle(
	std::unique_ptr<File> file, std::string file_path) noexcept {
	AssetsBundleVersion version;
	file->read(reinterpret_cast<unsigned char*>(&version), sizeof(version));
	AssetsBundleMapInfo map_info;
	file->read(reinterpret_cast<unsigned char*>(&map_info), sizeof(map_info));
	file->seek(static_cast<int>(map_info.position));
	std::string resource_map_json(map_info.size, '\0');
	file->read(
		reinterpret_cast<unsigned char*>(resource_map_json.data()),
		map_info.size);
	AssetsMap resource_map;
	try {
		resource_map = json::parse(resource_map_json);
	} catch (std::exception& e) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load assets map JSON: " + std::string(e.what())));
	}

	return AssetsBundle(std::move(file), file_path, resource_map);
}

void Growl::to_json(json& j, const AssetInfo& r) {
	j = json{{"position", r.position}, {"size", r.size}, {"typ", r.type}};
	if (r.atlas_regions.has_value()) {
		j["regions"] = r.atlas_regions.value();
	}
	if (r.font.has_value()) {
		j["font"] = r.font.value();
	}
	if (r.shader_pack.has_value()) {
		j["shaderPack"] = r.shader_pack.value();
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
	if (j.contains("shaderPack")) {
		r.shader_pack = j.at("shaderPack").get<AssetsBundleShaderPackInfo>();
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

void Growl::to_json(json& j, const AssetsBundleShaderPackInfo& r) {
	j = json{{"name", r.name}, {"sources", r.sources}};
}

void Growl::from_json(const json& j, AssetsBundleShaderPackInfo& r) {
	j.at("name").get_to(r.name);
	j.at("sources").get_to(r.sources);
}

void Growl::to_json(json& j, const AssetsBundleShaderSourceInfo& r) {
	j = json{
		{"vertexPos", r.vertex_pos},
		{"vertexSize", r.vertex_size},
		{"fragmentPos", r.fragment_pos},
		{"fragmentSize", r.fragment_size},
	};
}

void Growl::from_json(const json& j, AssetsBundleShaderSourceInfo& r) {
	j.at("vertexPos").get_to(r.vertex_pos);
	j.at("vertexSize").get_to(r.vertex_size);
	j.at("fragmentPos").get_to(r.fragment_pos);
	j.at("fragmentSize").get_to(r.fragment_size);
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
	case AssetType::ShaderPack:
		return "Shader Pack";
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
	file->seek(static_cast<int>(info.position));
	file->read(img_data.data(), info.size);

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
	file->seek(static_cast<int>(info.position));
	file->read(img_data.data(), info.size);

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
	file->seek(static_cast<int>(info.position));
	file->read(font_data.data(), info.size);

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
	file->seek(static_cast<int>(info.position));
	file->read(font_data.data(), info.size);

	if (!info.font.has_value()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load MSDF data from font"));
	}

	std::vector<unsigned char> image_data;
	image_data.resize(info.font.value().msdf_size);
	file->seek(static_cast<int>(info.font.value().msdf_position));
	file->read(image_data.data(), info.font.value().msdf_size);

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
	file->seek(static_cast<int>(info.position));
	file->read(reinterpret_cast<unsigned char*>(data.data()), info.size);

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
	file->seek(static_cast<int>(info.position));
	file->read(data.data(), info.size);

	return std::move(data);
}

Result<std::unique_ptr<File>>
AssetsBundle::getAssetAsFile(SystemAPI& system, std::string name) noexcept {
	auto info_find = assetsMap.find(name);
	if (info_find == assetsMap.end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load asset " + name +
			" as file; not found in asset map."));
	}
	auto& info = info_find->second;

	auto file_result =
		system.openFile(path, info.position, info.position + info.size);
	if (file_result.hasError()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to open file " + path + ": " +
			file_result.error()->message()));
	}
	return std::move(file_result.get());
}
