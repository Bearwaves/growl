#include "growl/core/assets/local_file.h"
#include "growl/core/assets/error.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include <memory>

using Growl::File;
using Growl::LocalFile;
using Growl::Result;

LocalFile::LocalFile(std::ifstream fp, size_t start, size_t end)
	: fp{std::move(fp)}
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
