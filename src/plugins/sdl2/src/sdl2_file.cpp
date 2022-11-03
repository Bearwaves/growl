#include "sdl2_file.h"
#include <cstdio>

using Growl::SDL2File;

SDL2File::SDL2File(SDL_RWops* fp, size_t start, size_t end)
	: fp{fp}
	, ptr{start}
	, start{start}
	, end{end} {}

SDL2File::~SDL2File() {
	if (fp) {
		SDL_RWclose(fp);
		fp = nullptr;
	}
}

bool SDL2File::eof() {
	return ptr == end;
}

size_t SDL2File::read(unsigned char* dst, size_t n_bytes) {
	SDL_RWseek(fp, ptr, SEEK_SET);
	int n = SDL_RWread(fp, dst, 1, n_bytes);
	if (n > 0) {
		ptr += n;
	}
	return n;
}

size_t SDL2File::length() {
	return end - start;
}

void SDL2File::seek(int offset) {
	size_t seek_to = offset < 0 ? end + offset : start + offset;
	if (seek_to < start) {
		seek_to = start;
	} else if (seek_to > end) {
		seek_to = end;
	}
	SDL_RWseek(fp, seek_to, SEEK_SET);
	ptr = seek_to;
}

size_t SDL2File::pos() {
	return ptr - start;
}
