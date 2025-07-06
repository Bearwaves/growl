#include "growl/core/assets/local_file.h"
#include "growl/core/error.h"

using Growl::File;
using Growl::LocalFile;
using Growl::Result;

LocalFile::LocalFile(
	std::string path, std::ifstream fp, size_t start, size_t end)
	: path{path}
	, fp{std::move(fp)}
	, start{start}
	, end{end}
	, ptr{start} {
	this->fp.seekg(start);
}

bool LocalFile::eof() {
	return ptr >= end;
}

size_t LocalFile::read(unsigned char* dst, size_t n_bytes) {
	if (n_bytes > end - ptr) {
		n_bytes = end - ptr;
	}
	fp.read(reinterpret_cast<char*>(dst), n_bytes);
	auto n = fp.gcount();
	ptr += n;
	return n;
}

size_t LocalFile::length() {
	return end - start;
}

void LocalFile::seek(int offset) {
	size_t seek_to = offset < 0 ? end + offset : start + offset;
	if (seek_to < start) {
		seek_to = start;
	} else if (seek_to > end) {
		seek_to = end;
	}
	fp.seekg(seek_to, std::ios::beg);
	ptr = seek_to;
}

size_t LocalFile::pos() {
	return ptr - start;
}

Result<std::unique_ptr<File>>
LocalFile::getRegionAsFile(size_t start, size_t length) {
	std::ifstream file;
	file.open(path, std::ios::binary | std::ios::in);
	if (file.fail()) {
		return Error(
			std::make_unique<GenericError>("Failed to open file " + path));
	}
	file.seekg(start, file.beg);
	auto ptr = file.tellg();
	file.seekg(start + length, file.beg);
	auto end = file.tellg();
	file.seekg(ptr);
	return std::unique_ptr<File>(
		std::make_unique<LocalFile>(path, std::move(file), start, end));
}
