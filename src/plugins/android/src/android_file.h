#pragma once

#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include <android/asset_manager.h>
#include <cstddef>

namespace Growl {

class AndroidFile : public File {
public:
	explicit AndroidFile(AAsset* asset, int fd, size_t start, size_t length);
	~AndroidFile();

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;
	Result<std::unique_ptr<File>>
	getRegionAsFile(size_t pos, size_t length) override;

private:
	AAsset* asset;
	int fd;
	size_t start;
	size_t len;
};

// Represents a view into a file without opening a new asset or FD.
class AndroidFileView : public File {
public:
	explicit AndroidFileView(int fd, size_t start, size_t length);

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;
	Result<std::unique_ptr<File>>
	getRegionAsFile(size_t pos, size_t length) override;

private:
	int fd;
	size_t start;
	size_t len;
	size_t ptr;
};

} // namespace Growl
