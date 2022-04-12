#include "../../../../thirdparty/fpng/fpng.h"
#include "../../../../thirdparty/stb_image/stb_image.h"
#include "../thirdparty/rang.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <growl/util/assets/bundle.h>
#include <growl/util/assets/error.h>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::AssetType;
using Growl::Error;
using Growl::Image;
using nlohmann::json;
using rang::fg;
using rang::style;
using std::cout;
using std::endl;

Error includeAtlas(
	const std::filesystem::path path, std::filesystem::path& resolved_path,
	AssetsMap& assets_map, std::ofstream& outfile) noexcept;

Error includeImage(
	const std::filesystem::directory_entry& entry,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept {
	if (!entry.is_regular_file()) {
		return nullptr;
	}
	int width, height, channels;
	unsigned char* img =
		stbi_load(entry.path().c_str(), &width, &height, &channels, 4);
	if (!img) {
		// Not an image.
		return nullptr;
	}
	Image image(width, height, channels, img);
	std::vector<uint8_t> out_buf;
	if (!fpng::fpng_encode_image_to_memory(
			img, width, height, 4, out_buf, fpng::FPNG_ENCODE_SLOWER)) {
		return std::make_unique<AssetsError>("Failed to encode image.");
	}
	unsigned int ptr = outfile.tellp();
	assets_map[resolved_path] = {ptr, out_buf.size(), AssetType::Image};
	outfile.write(
		reinterpret_cast<const char*>(out_buf.data()), out_buf.size());

	std::cout << "Included " << style::bold << resolved_path.string()
			  << style::reset << "." << endl;
	return nullptr;
}

Error processDirectory(
	std::string& assets_dir, std::filesystem::path path, AssetsMap& assets_map,
	std::ofstream& outfile) {
	std::filesystem::path dir_resolved_path =
		std::filesystem::relative(path, assets_dir);
	if (std::filesystem::exists(path / "atlas.json")) {
		cout << "Building atlas for " << style::bold
			 << dir_resolved_path.string() << style::reset << "." << endl;
		if (auto err =
				includeAtlas(path, dir_resolved_path, assets_map, outfile);
			err) {
			return std::make_unique<AssetsError>(
				"Failed to build atlas: " + err->message());
		}
		return nullptr;
	}
	for (const auto& file_entry : std::filesystem::directory_iterator(path)) {
		std::filesystem::path resolved_path =
			std::filesystem::relative(file_entry, assets_dir);
		if (auto err =
				includeImage(file_entry, resolved_path, assets_map, outfile);
			err) {
			cout << fg::red << "Failed to include asset "
				 << resolved_path.string() << ": " << err->message()
				 << style::reset << endl;
			exit(1);
		}
	}
	return nullptr;
}

void bundleAssets(std::string assets_dir, std::string output) noexcept {
	fpng::fpng_init();

	cout << "Building assets in " << style::bold << assets_dir << style::reset
		 << " to " << style::bold << output << style::reset << "." << endl;

	AssetsMap assets_map;

	std::ofstream outfile(
		output, std::ios::binary | std::ios::out | std::ios::trunc);
	AssetsBundleVersion version = Growl::ASSETS_BUNDLE_LATEST_VERSION;
	outfile.write(reinterpret_cast<const char*>(&version), sizeof(version));
	AssetsBundleMapInfo info{0, 0};
	outfile.write(reinterpret_cast<const char*>(&info), sizeof(info));

	std::filesystem::path assets_path(assets_dir);
	if (auto err =
			processDirectory(assets_dir, assets_path, assets_map, outfile);
		err) {
		cout << fg::red << "Failed to build asset bundle: " << err->message()
			 << style::reset << endl;
		exit(1);
	}
	for (const auto& dir_entry :
		 std::filesystem::recursive_directory_iterator(assets_dir)) {
		if (!std::filesystem::is_directory(dir_entry)) {
			continue;
		}
		if (auto err = processDirectory(
				assets_dir, dir_entry.path(), assets_map, outfile);
			err) {
			cout << fg::red
				 << "Failed to build asset bundle: " << err->message()
				 << style::reset << endl;
			exit(1);
		}
	}

	info.position = outfile.tellp();
	json resource_map_json = assets_map;
	auto resource_map_encoded = resource_map_json.dump();
	info.size = resource_map_encoded.size();
	outfile.write(
		reinterpret_cast<const char*>(resource_map_encoded.c_str()),
		resource_map_encoded.size());
	outfile.seekp(sizeof(version));
	outfile.write(reinterpret_cast<const char*>(&info), sizeof(info));
}

void listAssets(std::string assets_bundle) {
	auto assets_bundle_result = Growl::loadAssetsBundle(assets_bundle);
	if (assets_bundle_result.hasError()) {
		cout << fg::red << "Failed to load asset bundle: "
			 << assets_bundle_result.error()->message() << style::reset << endl;
		exit(1);
	}
	auto assets_map = assets_bundle_result.get().getAssetsMap();
	cout << "Found " << style::bold << assets_map.size() << style::reset
		 << " assets." << endl;
	for (auto [asset, info] : assets_map) {
		cout << " • [" << style::bold << Growl::getAssetTypeName(info.type)
			 << style::reset << "] " << asset << endl;
		if (info.type == AssetType::Atlas && info.atlas_regions.has_value()) {
			cout << "   Atlas contains " << style::bold
				 << info.atlas_regions.value().size() << style::reset
				 << " images." << endl;
			for (auto& [name, _] : info.atlas_regions.value()) {
				cout << "    • " << name << endl;
			}
		}
	}
}
