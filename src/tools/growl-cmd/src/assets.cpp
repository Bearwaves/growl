#include "../../../contrib/fpng/fpng.h"
#include "../../../contrib/stb_image/stb_image.h"
#include "../contrib/rang.hpp"
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <string>

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

	json resourceMap;

	std::ofstream outfile(output, std::ios::binary | std::ios::out);
	uint64_t size = 0;
	outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));

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
		resourceMap[resolved_path] = {ptr, out_buf.size()};
		outfile.write(
			reinterpret_cast<const char*>(out_buf.data()), out_buf.size());
		std::cout << "Included " << style::bold << resolved_path.string()
				  << style::reset << "." << endl;
	}
	size = outfile.tellp();
	outfile.write(
		reinterpret_cast<const char*>(resourceMap.dump().c_str()),
		resourceMap.dump().size());
	outfile.seekp(0);
	outfile.write(reinterpret_cast<const char*>(&size), sizeof(size));
}
