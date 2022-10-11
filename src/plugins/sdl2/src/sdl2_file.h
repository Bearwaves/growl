#pragma once

#include "SDL.h"
#include "growl/core/assets/file.h"
#include <cstddef>
namespace Growl {

class SDL2File : public File {
public:
	explicit SDL2File(SDL_RWops* fp, size_t start, size_t end);
	~SDL2File();

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;

private:
	SDL_RWops* fp;
	size_t ptr;
	size_t start;
	size_t end;
};

} // namespace Growl
