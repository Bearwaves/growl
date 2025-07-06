#include "sdl3_file.h"
#include "SDL3/SDL_iostream.h"
#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include "sdl3_error.h"

using Growl::File;
using Growl::Result;
using Growl::SDL3Error;
using Growl::SDL3File;

SDL3File::SDL3File(std::string path, SDL_IOStream* fp, size_t start, size_t end)
	: path{path}
	, fp{fp}
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

Result<std::unique_ptr<File>>
SDL3File::getRegionAsFile(size_t start, size_t length) {
	// We just open the file again, it's fine, probably
	auto new_fp = SDL_IOFromFile(path.c_str(), "rb");

	if (!new_fp) {
		return Error(std::make_unique<SDL3Error>(SDL_GetError()));
	}

	return std::unique_ptr<File>(
		std::make_unique<SDL3File>(path, new_fp, start, start + length));
}
