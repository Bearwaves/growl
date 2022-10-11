#include "file.h"
#include "error.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include <memory>

using Growl::File;
using Growl::LocalFile;
using Growl::Result;

Result<std::unique_ptr<File>> Growl::openLocalFile(std::string path) {
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);
	if (file.fail()) {
		return Error(
			std::make_unique<AssetsError>("Failed to open file " + path));
	}
	return std::unique_ptr<File>(std::make_unique<LocalFile>(std::move(file)));
}

LocalFile::LocalFile(std::ifstream fp)
	: fp{std::move(fp)} {}

LocalFile::~LocalFile() {
	if (fp.is_open()) {
		fp.close();
	}
}

bool LocalFile::eof() {
	return fp.eof();
}

size_t LocalFile::read(unsigned char* dst, size_t n_bytes) {
	return fp.readsome(reinterpret_cast<char*>(dst), n_bytes);
}

size_t LocalFile::length() {
	auto ptr = pos();
	fp.seekg(0, std::ios::end);
	auto new_ptr = pos();
	seek(ptr);
	return new_ptr;
}

void LocalFile::seek(int offset) {
	auto seek_from = offset < 0 ? std::ios::end : std::ios::beg;
	fp.seekg(offset, seek_from);
}

size_t LocalFile::pos() {
	return fp.tellg();
}
