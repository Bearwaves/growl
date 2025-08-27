#include "android_file.h"
#include "android_error.h"
#include <cstdio>
#include <stddef.h>
#include <unistd.h>

using Growl::AndroidError;
using Growl::AndroidFile;
using Growl::AndroidFileView;
using Growl::File;
using Growl::Result;

AndroidFile::AndroidFile(AAsset* asset, int fd, size_t start, size_t length)
	: asset{asset}
	, fd{fd}
	, start{start}
	, len{length} {}

AndroidFile::~AndroidFile() {
	if (asset) {
		AAsset_close(asset);
		asset = nullptr;
	}
}

bool AndroidFile::eof() {
	return AAsset_getRemainingLength(asset) == 0;
}

size_t AndroidFile::read(unsigned char* dst, size_t n_bytes) {
	if (n_bytes > AAsset_getRemainingLength(asset)) {
		n_bytes = AAsset_getRemainingLength(asset);
	}
	return AAsset_read(asset, dst, n_bytes);
}

size_t AndroidFile::length() {
	return AAsset_getLength(asset);
}

void AndroidFile::seek(int offset) {
	AAsset_seek64(this->asset, offset, SEEK_SET);
}

size_t AndroidFile::pos() {
	return AAsset_getLength(asset) - AAsset_getRemainingLength(asset);
}

Result<std::unique_ptr<File>>
AndroidFile::getRegionAsFile(size_t pos, size_t length) {
	if (pos > len || pos + length > len) {
		return Error(
			std::make_unique<AndroidError>("Region out of range of file"));
	}
	return std::unique_ptr<File>(
		std::make_unique<AndroidFileView>(fd, start + pos, length));
}

AndroidFileView::AndroidFileView(int fd, size_t start, size_t length)
	: fd{fd}
	, start{start}
	, len{length}
	, ptr{start} {}

bool AndroidFileView::eof() {
	return ptr >= start + len;
}

size_t AndroidFileView::read(unsigned char* dst, size_t n_bytes) {
	if (n_bytes > start + len - ptr) {
		n_bytes = start + len - ptr;
	}
	auto n = pread(fd, dst, n_bytes, ptr);
	ptr += n;
	return n;
}

size_t AndroidFileView::length() {
	return len;
}

void AndroidFileView::seek(int offset) {
	size_t seek_to = offset < 0 ? (start + len) - offset : start + offset;
	if (seek_to < start) {
		seek_to = start;
	} else if (seek_to > start + len) {
		seek_to = start + len;
	}
	ptr = seek_to;
}

size_t AndroidFileView::pos() {
	return ptr - start;
}

Result<std::unique_ptr<File>>
AndroidFileView::getRegionAsFile(size_t pos, size_t length) {
	return Error(
		std::make_unique<AndroidError>(
			"Cannot create file region from file region"));
}
