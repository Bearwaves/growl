#include "growl/util/assets/atlas.h"
#include "../../../../thirdparty/fpng/fpng.h"
#include "../../../../thirdparty/stb_image/stb_image.h"
#include "../thirdparty/rang.hpp"
#include "growl/util/assets/bundle.h"
#include "growl/util/assets/error.h"
#include "growl/util/error.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <ostream>

using Growl::AssetsError;
using Growl::AssetsMap;
using Growl::AssetType;
using Growl::AtlasImagePackInfo;
using Growl::Error;
using rang::style;

Error includeAtlas(
	std::filesystem::path path, std::filesystem::path& resolved_path,
	AssetsMap& assets_map, std::ofstream& outfile) noexcept {
	int padding = 0;
	std::ifstream json_file(path / "atlas.json");
	try {
		json j = json::parse(json_file);
		padding = j.value("padding", padding);
	} catch (std::exception&) {
		return std::make_unique<AssetsError>("Failed to read pack.json");
	}

	std::vector<Growl::AtlasImagePackInfo> images;
	for (auto entry : std::filesystem::directory_iterator(path)) {
		int width, height, channels;
		if (!stbi_info(entry.path().string().c_str(), &width, &height, &channels)) {
			// Not an image
			continue;
		}
		images.push_back(AtlasImagePackInfo(entry.path(), width, height));
	}

	auto result = packAtlasFromFiles(images, padding);
	if (result.hasError()) {
		return std::move(result.error());
	}
	auto atlas = std::move(result.get());
	for (auto& [name, _] : atlas.getMappings()) {
		std::cout << "=> [" << style::bold << resolved_path.string()
				  << style::reset << "] Included image " << style::bold << name
				  << style::reset << "." << std::endl;
	}

	std::vector<uint8_t> out_buf;
	if (!fpng::fpng_encode_image_to_memory(
			atlas.getImage().getRaw(), atlas.getImage().getWidth(),
			atlas.getImage().getHeight(), 4, out_buf,
			fpng::FPNG_ENCODE_SLOWER)) {
		return std::make_unique<AssetsError>("Failed to encode image.");
	}
	auto ptr = static_cast<unsigned int>(outfile.tellp());
	assets_map[resolved_path.string()] = {
		ptr, out_buf.size(), AssetType::Atlas, atlas.getMappings()};
	outfile.write(
		reinterpret_cast<const char*>(out_buf.data()), out_buf.size());

	return nullptr;
}
