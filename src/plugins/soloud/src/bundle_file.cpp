#include "bundle_file.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/error.h"
#include "growl/core/error.h"
#include <fstream>
#include <string_view>

using Growl::AssetsBundle;
using Growl::AssetsError;
using Growl::Result;
using Growl::SoLoudBundleFile;

int SoLoudBundleFile::eof() {
	return file.tellg() >= offset + size;
}

unsigned int SoLoudBundleFile::read(unsigned char* dst, unsigned int n_bytes) {
	if ((unsigned int)file.tellg() + n_bytes >= offset + size) {
		n_bytes = (offset + size) - file.tellg();
	}
	file.read(reinterpret_cast<char*>(dst), n_bytes);
	return file.gcount();
}

unsigned int SoLoudBundleFile::length() {
	return size;
}

void SoLoudBundleFile::seek(int offset) {
	unsigned int seek_to =
		offset < 0 ? (this->offset + size) + offset : this->offset + offset;
	if (seek_to < this->offset) {
		seek_to = this->offset;
	} else if (seek_to > this->offset + size - 1) {
		seek_to = this->offset + size - 1;
	}
	file.seekg(seek_to);
}

unsigned int SoLoudBundleFile::pos() {
	return (unsigned int)file.tellg() - offset;
}

Result<std::unique_ptr<SoLoudBundleFile>>
Growl::openFileFromBundle(AssetsBundle& bundle, std::string name) noexcept {
	Result<std::ifstream> file_result = bundle.openNewStream();
	if (file_result.hasError()) {
		return std::move(file_result.error());
	}
	std::ifstream file = std::move(file_result.get());
	auto info_find = bundle.getAssetsMap().find(name);
	if (info_find == bundle.getAssetsMap().end()) {
		return Error(std::make_unique<AssetsError>(
			"Failed to load file " + name +
			" from bundle; not found in asset map."));
	}
	auto& info = info_find->second;
	file.seekg(info.position);
	return std::make_unique<SoLoudBundleFile>(
		std::move(file), info.position, info.size);
}
