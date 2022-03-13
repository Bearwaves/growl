#include "../../../contrib/fpng/fpng.h"
#include "../../../contrib/stb_image/stb_image.h"
#include "../contrib/rang.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <growl/util/assets/bundle.h>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsMap;
using Growl::AssetType;
using nlohmann::json;
using rang::fg;
using rang::style;
using std::cout;
using std::endl;
using std::filesystem::is_regular_file;
using std::filesystem::recursive_directory_iterator;

void bundleAssets(std::string assets_dir, std::string output) noexcept {
	fpng::fpng_init();

	cout << "Building assets in " << style::bold << assets_dir << style::reset
		 << " to " << style::bold << output << style::reset << "." << endl;

	AssetsMap resourceMap;

	std::ofstream outfile(
		output, std::ios::binary | std::ios::out | std::ios::trunc);
	AssetsBundleVersion version = Growl::ASSETS_BUNDLE_LATEST_VERSION;
	outfile.write(reinterpret_cast<const char*>(&version), sizeof(version));
	AssetsBundleMapInfo info{0, 0};
	outfile.write(reinterpret_cast<const char*>(&info), sizeof(info));

	for (const auto& entry : recursive_directory_iterator(assets_dir)) {
		if (!is_regular_file(entry)) {
			continue;
		}
		std::filesystem::path resolved_path =
			std::filesystem::relative(entry, assets_dir);

		int width, height, channels;
		unsigned char* img =
			stbi_load(entry.path().c_str(), &width, &height, &channels, 4);
		if (!img) {
			cout << fg::red << "Failed to load image " << style::bold
				 << resolved_path.string() << style::reset << fg::red
				 << ". Exiting." << style::reset << endl;
			exit(1);
		}
		std::vector<uint8_t> out_buf;
		if (!fpng::fpng_encode_image_to_memory(
				img, width, height, 4, out_buf, fpng::FPNG_ENCODE_SLOWER)) {
			cout << fg::red << "Failed to encode image." << style::reset
				 << endl;
			exit(1);
		}
		unsigned int ptr = outfile.tellp();
		resourceMap[resolved_path] = {ptr, out_buf.size(), AssetType::Image};
		outfile.write(
			reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
		std::cout << "Included " << style::bold << resolved_path.string()
				  << style::reset << "." << endl;
	}
	info.position = outfile.tellp();
	json resourceMapJson = resourceMap;
	auto resourceMapEncoded = resourceMapJson.dump();
	info.size = resourceMapEncoded.size();
	outfile.write(
		reinterpret_cast<const char*>(resourceMapEncoded.c_str()),
		resourceMapEncoded.size());
	outfile.seekp(sizeof(version));
	outfile.write(reinterpret_cast<const char*>(&info), sizeof(info));
}

void listAssets(std::string assetsBundle) {
	auto assetsBundleResult = Growl::loadAssetsBundle(assetsBundle);
	if (assetsBundleResult.hasError()) {
		cout << fg::red << "Failed to load asset bundle: "
			 << assetsBundleResult.error()->message() << style::reset << endl;
		exit(1);
	}
	auto assetsMap = assetsBundleResult.get().getAssetsMap();
	cout << "Found " << style::bold << assetsMap.size() << style::reset
		 << " assets." << endl;
	for (auto [asset, info] : assetsMap) {
		cout << " • [" << style::bold << Growl::getAssetTypeName(info.type)
			 << style::reset << "] " << asset << endl;
	}
}
