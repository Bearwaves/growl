#pragma once

#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include <android/asset_manager.h>
#include <cstddef>

namespace Growl {

class AndroidFile : public File {
public:
	explicit AndroidFile(AAsset* asset, size_t start, size_t end);
	~AndroidFile();

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;

private:
	AAsset* asset;
	size_t start;
	size_t end;
	size_t ptr;
};

} // namespace Growl
