#pragma once

#include "growl/core/assets/file.h"
#include <cstddef>
#include <fstream>

namespace Growl {

class LocalFile : public File {
public:
	explicit LocalFile(std::ifstream fp, size_t start, size_t end);
	~LocalFile() = default;

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;

private:
	std::ifstream fp;
	size_t start;
	size_t end;
	size_t ptr;
};

} // namespace Growl
