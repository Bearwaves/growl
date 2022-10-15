#include "android_file.h"
#include <cstdio>
#include <stddef.h>

using Growl::AndroidFile;

AndroidFile::AndroidFile(AAsset* asset, size_t start, size_t end)
	: asset{asset}
	, start{start}
	, end{end}
	, ptr{start} {
	AAsset_seek64(this->asset, start, SEEK_SET);
}

AndroidFile::~AndroidFile() {
	if (asset) {
		AAsset_close(asset);
		asset = nullptr;
	}
}

bool AndroidFile::eof() {
	return ptr >= end;
}

size_t AndroidFile::read(unsigned char* dst, size_t n_bytes) {
	if (n_bytes > end - ptr) {
		n_bytes = end - ptr;
	}
	auto n = AAsset_read(asset, dst, n_bytes);
	ptr += n;
	return n;
}

size_t AndroidFile::length() {
	return end - start;
}

void AndroidFile::seek(int offset) {
	size_t seek_to = offset < 0 ? end + offset : start + offset;
	if (seek_to < start) {
		seek_to = start;
	} else if (seek_to > end) {
		seek_to = end;
	}
	AAsset_seek64(this->asset, seek_to, SEEK_SET);
	ptr = seek_to;
}

size_t AndroidFile::pos() {
	return ptr - start;
}
