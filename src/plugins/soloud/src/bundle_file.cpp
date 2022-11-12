#include "bundle_file.h"
#include "growl/core/assets/bundle.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include <fstream>

namespace Growl {
class AssetsError;
class SystemAPI;
} // namespace Growl

using Growl::AssetsBundle;
using Growl::AssetsError;
using Growl::File;
using Growl::Result;
using Growl::SoLoudBundleFile;
using Growl::SystemAPI;

int SoLoudBundleFile::eof() {
	return file->eof();
}

unsigned int SoLoudBundleFile::read(unsigned char* dst, unsigned int n_bytes) {
	return static_cast<unsigned int>(file->read(dst, n_bytes));
}

unsigned int SoLoudBundleFile::length() {
	return static_cast<unsigned int>(file->length());
}

void SoLoudBundleFile::seek(int offset) {
	file->seek(offset);
}

unsigned int SoLoudBundleFile::pos() {
	return static_cast<unsigned int>(file->pos());
}

Result<std::unique_ptr<SoLoudBundleFile>> Growl::openFileFromBundle(
	SystemAPI& system, AssetsBundle& bundle, std::string name) noexcept {
	Result<std::unique_ptr<File>> file_result =
		bundle.getAssetAsFile(system, name);
	if (file_result.hasError()) {
		return std::move(file_result.error());
	}
	return std::make_unique<SoLoudBundleFile>(std::move(file_result.get()));
}
