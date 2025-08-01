#include "../thirdparty/rang.hpp"
#include "assets_config.h"
#include "error.h"
#include "fpng.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/assets/font_face.h"
#include "growl/core/assets/local_file.h"
#include "growl/core/assets/shader_pack.h"
#include "nlohmann/json.hpp"
#include "soloud_wavstream.h"
#include "stb_image.h"
#include "utf8/core.h"
#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

using Growl::AssetConfig;
using Growl::AssetInfo;
using Growl::AssetsBundleMapInfo;
using Growl::AssetsBundleMSDFFontInfo;
using Growl::AssetsBundleVersion;
using Growl::AssetsError;
using Growl::AssetsIncludeError;
using Growl::AssetsIncludeErrorCode;
using Growl::AssetsMap;
using Growl::AssetType;
using Growl::AtlasConfig;
using Growl::Error;
using Growl::File;
using Growl::FontConfig;
using Growl::Image;
using Growl::LocalFile;
using Growl::Result;
using Growl::ShaderPackConfig;
using Growl::ShaderType;
using nlohmann::json;
using rang::fg;
using rang::style;
using std::cout;
using std::endl;

static constexpr const char* ASSETS_CONFIG_FILENAME = "assets.json";

bool isValidAudio(std::string path) {
	SoLoud::WavStream sound;
	return sound.load(path.c_str()) == SoLoud::SO_NO_ERROR;
}

AssetsIncludeError includeAtlas(
	const AtlasConfig& config, const std::filesystem::path& path,
	const std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept;

AssetsIncludeError includeShaderPack(
	const ShaderPackConfig& config, const std::filesystem::path& path,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept;

std::string shaderFormat(ShaderType type);

AssetsIncludeError includeImage(
	const std::filesystem::directory_entry& entry,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept {
	if (!entry.is_regular_file()) {
		return AssetsIncludeErrorCode::None;
	}
	int width, height, channels;
	unsigned char* img =
		stbi_load(entry.path().string().c_str(), &width, &height, &channels, 4);
	if (!img) {
		// Not an image.
		return AssetsIncludeErrorCode::WrongType;
	}
	Image image(width, height, channels, img);
	std::vector<uint8_t> out_buf;
	if (!fpng::fpng_encode_image_to_memory(
			img, width, height, 4, out_buf, fpng::FPNG_ENCODE_SLOWER)) {
		return AssetsIncludeError("Failed to encode image.");
	}
	auto ptr = static_cast<unsigned int>(outfile.tellp());
	assets_map[resolved_path.generic_string()] = {
		ptr, out_buf.size(), AssetType::Image};
	outfile.write(
		reinterpret_cast<const char*>(out_buf.data()), out_buf.size());

	std::cout << "Included " << style::bold << resolved_path.generic_string()
			  << style::reset << "." << endl;
	return AssetsIncludeErrorCode::None;
}

AssetsIncludeError includeFont(
	const std::filesystem::directory_entry& entry, const FontConfig& config,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept {
	// Try to create a font
	if (!Growl::isValidFont(entry.path().string())) {
		return AssetsIncludeErrorCode::WrongType;
	}

	auto size = std::filesystem::file_size(entry.path());
	std::vector<unsigned char> data(size);
	std::ifstream file;
	file.open(entry.path(), std::ios::binary | std::ios::in);
	if (file.fail()) {
		return AssetsIncludeError(
			"Failed to open file " + resolved_path.generic_string());
	}
	file.read(reinterpret_cast<char*>(data.data()), size);

	auto ptr = static_cast<unsigned int>(outfile.tellp());
	AssetInfo info{ptr, size, AssetType::Font};
	outfile.write(reinterpret_cast<const char*>(data.data()), size);

	std::cout << "Included font " << style::bold
			  << resolved_path.generic_string() << style::reset << "." << endl;

	if (config.msdf) {
		std::cout << "Generating MSDF font atlas..." << std::endl;
		auto dist_result = Growl::createDistanceFieldFontFaceFromFile(
			entry.path().string(), config.msdfSize, config.msdfCharacters);
		if (dist_result.hasError()) {
			return AssetsIncludeError(
				"failed to create MSDF font: " +
				dist_result.error()->message());
		}
		std::vector<uint8_t> out_buf;
		auto& image = dist_result.get().getImage();
		if (!fpng::fpng_encode_image_to_memory(
				image.getRaw(), image.getWidth(), image.getHeight(),
				image.getChannels(), out_buf, fpng::FPNG_ENCODE_SLOWER)) {
			return AssetsIncludeError("Failed to encode MSDF image.");
		}
		info.font = AssetsBundleMSDFFontInfo{
			static_cast<unsigned int>(outfile.tellp()), out_buf.size(),
			dist_result.get().getGlyphs(), dist_result.get().getPixelRange()};
		outfile.write(
			reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
		std::cout << "Included MSDF font atlas for " << style::bold
				  << resolved_path.generic_string() << style::reset << "."
				  << std::endl;
	}
	assets_map[resolved_path.generic_string()] = info;

	return AssetsIncludeErrorCode::None;
}

AssetsIncludeError includeAudio(
	const std::filesystem::directory_entry& entry,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept {
	if (!isValidAudio(entry.path().string())) {
		return AssetsIncludeErrorCode::WrongType;
	}

	auto size = std::filesystem::file_size(entry.path());
	std::vector<unsigned char> data(size);
	std::ifstream file;
	file.open(entry.path(), std::ios::binary | std::ios::in);
	if (file.fail()) {
		return AssetsIncludeError(
			"Failed to open file " + resolved_path.generic_string());
	}
	file.read(reinterpret_cast<char*>(data.data()), size);

	auto ptr = static_cast<unsigned int>(outfile.tellp());
	AssetInfo info{ptr, size, AssetType::Audio};
	outfile.write(reinterpret_cast<const char*>(data.data()), size);

	std::cout << "Included audio " << style::bold
			  << resolved_path.generic_string() << style::reset << "."
			  << std::endl;

	assets_map[resolved_path.generic_string()] = info;

	return AssetsIncludeErrorCode::None;
}

AssetsIncludeError includeText(
	const std::filesystem::directory_entry& entry,
	std::filesystem::path& resolved_path, AssetsMap& assets_map,
	std::ofstream& outfile) noexcept {
	std::ifstream file;
	file.open(entry.path(), std::ios::in);
	if (file.fail()) {
		return AssetsIncludeError(
			"Failed to open file " + resolved_path.generic_string());
	}

	auto start = static_cast<unsigned int>(outfile.tellp());
	std::string line;
	// Check for valid UTF-8 and convert Windows line endings to Unix.
	while (std::getline(file, line)) {
		std::string::size_type pos = line.find_last_not_of("\r\n");
		line.erase(pos + 1);
		if (!utf8::is_valid(line.begin(), line.end())) {
			return AssetsIncludeError("Line " + line + " is not valid UTF-8.");
		}
		outfile << line << '\n';
	}

	AssetInfo info{
		start, static_cast<unsigned int>(outfile.tellp()) - start,
		AssetType::Text};
	std::cout << "Included text file " << style::bold
			  << resolved_path.generic_string() << style::reset << "."
			  << std::endl;

	assets_map[resolved_path.generic_string()] = info;

	return AssetsIncludeErrorCode::None;
}

Result<bool> processDirectory(
	std::string& assets_dir, std::filesystem::path path, AssetsMap& assets_map,
	std::ofstream& outfile) {
	std::filesystem::path dir_resolved_path =
		std::filesystem::relative(path, assets_dir);
	std::unordered_map<std::string, AssetConfig> config;
	if (std::filesystem::exists(path / ASSETS_CONFIG_FILENAME)) {
		try {
			std::ifstream config_file(path / ASSETS_CONFIG_FILENAME);
			config = json::parse(config_file);
		} catch (std::exception& e) {
			return Error(
				std::make_unique<AssetsError>(
					"Failed to parse assets.json: " + std::string(e.what())));
		}
	}
	if (auto it = config.find("."); it != config.end()) {
		if (it->second.atlas.has_value()) {
			cout << "Building atlas for " << style::bold
				 << dir_resolved_path.generic_string() << style::reset << "."
				 << endl;
			if (auto err = includeAtlas(
					it->second.atlas.value(), path, dir_resolved_path,
					assets_map, outfile);
				err.getCode() == AssetsIncludeErrorCode::LoadFailed) {
				return Error(
					std::make_unique<AssetsError>(
						"Failed to build atlas: " + err.message()));
			}
			if (it->second.atlas->recursive) {
				return false;
			}
		}
		if (it->second.shader_pack.has_value()) {
			cout << "Building shader pack for " << style::bold
				 << dir_resolved_path.generic_string() << style::reset << "."
				 << endl;
			if (auto err = includeShaderPack(
					it->second.shader_pack.value(), path, dir_resolved_path,
					assets_map, outfile);
				err.getCode() == AssetsIncludeErrorCode::LoadFailed) {
				return Error(
					std::make_unique<AssetsError>(
						"Failed to build shader pack: " + err.message()));
			}
		}
		return true;
	}
	for (const auto& file_entry : std::filesystem::directory_iterator(path)) {
		std::filesystem::path resolved_path =
			std::filesystem::relative(file_entry, assets_dir);

		AssetConfig asset_config;
		if (auto it =
				config.find(file_entry.path().filename().generic_string());
			it != config.end()) {
			asset_config = it->second;
		} else if (auto it = config.find("*");
				   it != config.end() &&
				   file_entry.path().filename().generic_string() !=
					   ASSETS_CONFIG_FILENAME) {
			asset_config = it->second;
		}

		if (asset_config.text) {
			// Attempt to include file as plain text
			if (auto err =
					includeText(file_entry, resolved_path, assets_map, outfile);
				err.getCode() == AssetsIncludeErrorCode::LoadFailed) {
				return Error(
					std::make_unique<AssetsError>(
						"Failed to include text file: " + err.message()));
			}
			continue;
		}

		// Try to import things as different asset types
		auto img_err =
			includeImage(file_entry, resolved_path, assets_map, outfile);
		if (img_err.getCode() == AssetsIncludeErrorCode::None) {
			continue;
		}
		if (img_err.getCode() == AssetsIncludeErrorCode::LoadFailed) {
			return Error(
				std::make_unique<AssetsError>(
					"Failed to include image: " + img_err.message()));
		}

		FontConfig font_config;
		if (asset_config.font.has_value()) {
			font_config = asset_config.font.value();
		}
		auto font_err = includeFont(
			file_entry, font_config, resolved_path, assets_map, outfile);
		if (font_err.getCode() == AssetsIncludeErrorCode::None) {
			continue;
		}
		if (font_err.getCode() == AssetsIncludeErrorCode::LoadFailed) {
			return Error(
				std::make_unique<AssetsError>(
					"Failed to include font: " + font_err.message()));
		}

		auto sound_err =
			includeAudio(file_entry, resolved_path, assets_map, outfile);
		if (sound_err.getCode() == AssetsIncludeErrorCode::None) {
			continue;
		}
		if (sound_err.getCode() == AssetsIncludeErrorCode::LoadFailed) {
			return Error(
				std::make_unique<AssetsError>(
					"Failed to include sound: " + sound_err.message()));
		}
	}
	return true;
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
	auto res = processDirectory(assets_dir, assets_path, assets_map, outfile);
	if (res.hasError()) {
		cout << fg::red
			 << "Failed to build asset bundle: " << res.error()->message()
			 << style::reset << endl;
		exit(1);
	}
	if (!*res) {
		return;
	}
	auto iter = std::filesystem::recursive_directory_iterator(assets_dir);
	for (const auto& dir_entry : iter) {
		if (!std::filesystem::is_directory(dir_entry)) {
			continue;
		}
		res =
			processDirectory(assets_dir, dir_entry.path(), assets_map, outfile);
		if (res.hasError()) {
			cout << fg::red
				 << "Failed to build asset bundle: " << res.error()->message()
				 << style::reset << endl;
			exit(1);
		}
		if (!*res) {
			iter.disable_recursion_pending();
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

Result<std::unique_ptr<File>> openLocalFile(std::string path) {
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);
	if (file.fail()) {
		return Error(
			std::make_unique<AssetsError>("Failed to open file " + path));
	}
	auto ptr = file.tellg();
	file.seekg(0, file.end);
	auto end = file.tellg();
	file.seekg(ptr);
	return std::unique_ptr<File>(
		std::make_unique<LocalFile>(path, std::move(file), 0, end));
}

void listAssets(std::string assets_bundle) {
	auto file_result = openLocalFile(assets_bundle);
	if (file_result.hasError()) {
		cout << fg::red
			 << "Failed to load file: " << file_result.error()->message()
			 << style::reset << endl;
		exit(1);
	}
	auto assets_bundle_result =
		Growl::loadAssetsBundle(std::move(file_result.get()), assets_bundle);
	if (assets_bundle_result.hasError()) {
		cout << fg::red << "Failed to load asset bundle: "
			 << assets_bundle_result.error()->message() << style::reset << endl;
		exit(1);
	}
	auto& assets_map = assets_bundle_result.get().getAssetsMap();
	cout << "Found " << style::bold << assets_map.size() << style::reset
		 << " assets." << endl;
	for (auto& [asset, info] : assets_map) {
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
		if (info.type == AssetType::Font && info.font.has_value()) {
			cout << "   Includes MSDF atlas with "
				 << info.font.value().glyphs.size() << " glyphs." << endl;
		}
		if (info.type == AssetType::ShaderPack &&
			info.shader_pack.has_value()) {
			cout << "   Shader Pack contains " << style::bold
				 << info.shader_pack.value().sources.size() << style::reset
				 << " source formats." << endl;
			for (auto& [type, sources] : info.shader_pack.value().sources) {
				cout << "    • " << shaderFormat(type) << style::reset << ": ";
				if (sources.vertex_size && sources.fragment_size) {
					cout << "[vertex, fragment]" << endl;
				} else if (sources.fragment_size) {
					cout << "[fragment]" << endl;
				} else if (sources.vertex_size) {
					cout << "[vertex]" << endl;
				} else {
					cout << "[]" << endl;
				}
			}
		}
	}
}
