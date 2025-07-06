#pragma once

#include "SDL3/SDL_iostream.h"
#include "growl/core/assets/file.h"
#include <cstddef>
namespace Growl {

class SDL3File : public File {
public:
	explicit SDL3File(
		std::string path, SDL_IOStream* fp, size_t start, size_t end);
	~SDL3File();

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;

	Result<std::unique_ptr<File>>
	getRegionAsFile(size_t start, size_t length) override;

private:
	std::string path;
	SDL_IOStream* fp;
	size_t ptr;
	size_t start;
	size_t end;
};

} // namespace Growl
