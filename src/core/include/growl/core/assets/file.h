#pragma once
#include <cstddef>

namespace Growl {

class File {
public:
	explicit File() = default;
	virtual ~File() {}
	// File is move-only
	File(const File&) = delete;
	File& operator=(const File&) = delete;
	File(File&&) = default;
	File& operator=(File&&) = default;

	virtual bool eof() = 0;
	virtual size_t read(unsigned char* dst, size_t n_bytes) = 0;
	virtual size_t length() = 0;
	virtual void seek(int offset) = 0;
	virtual size_t pos() = 0;
};

} // namespace Growl
