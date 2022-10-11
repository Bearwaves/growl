#pragma once

#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include <cstddef>
#include <fstream>
#include <memory>
namespace Growl {

class LocalFile : public File {
public:
	explicit LocalFile(std::ifstream fp);
	~LocalFile();

	bool eof() override;
	size_t read(unsigned char* dst, size_t n_bytes) override;
	size_t length() override;
	void seek(int offset) override;
	size_t pos() override;

private:
	std::ifstream fp;
};

Result<std::unique_ptr<File>> openLocalFile(std::string path);

} // namespace Growl
