#include "sdl3_file.h"
#include <cstdio>

using Growl::SDL3File;

SDL3File::SDL3File(SDL_IOStream* fp, size_t start, size_t end)
	: fp{fp}
	, ptr{start}
	, start{start}
	, end{end} {}

SDL3File::~SDL3File() {
	if (fp) {
		SDL_CloseIO(fp);
		fp = nullptr;
	}
}

bool SDL3File::eof() {
	return ptr == end;
}

size_t SDL3File::read(unsigned char* dst, size_t n_bytes) {
	SDL_SeekIO(fp, ptr, SDL_IO_SEEK_SET);
	int n = n_bytes > 0 ? SDL_ReadIO(fp, dst, n_bytes) : 0;
	if (n > 0) {
		ptr += n;
	}
	return n;
}

size_t SDL3File::length() {
	return end - start;
}

void SDL3File::seek(int offset) {
	size_t seek_to = offset < 0 ? end + offset : start + offset;
	if (seek_to < start) {
		seek_to = start;
	} else if (seek_to > end) {
		seek_to = end;
	}
	SDL_SeekIO(fp, seek_to, SDL_IO_SEEK_SET);
	ptr = seek_to;
}

size_t SDL3File::pos() {
	return ptr - start;
}
