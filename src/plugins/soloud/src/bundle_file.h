#pragma once

#include "growl/core/assets/bundle.h"
#include "soloud_file.h"
#include <fstream>
namespace Growl {

class SoLoudBundleFile : public SoLoud::File {
public:
	explicit SoLoudBundleFile(
		std::ifstream file, unsigned int offset, unsigned int size)
		: file{std::move(file)}
		, offset{offset}
		, size{size} {}
	~SoLoudBundleFile() = default;

	// SoLoudBundleFile is move-only
	SoLoudBundleFile(const SoLoudBundleFile&) = delete;
	SoLoudBundleFile& operator=(const SoLoudBundleFile&) = delete;
	SoLoudBundleFile(SoLoudBundleFile&&) = default;
	SoLoudBundleFile& operator=(SoLoudBundleFile&&) = default;

	virtual int eof() override;
	virtual unsigned int
	read(unsigned char* dst, unsigned int n_bytes) override;
	virtual unsigned int length() override;
	virtual void seek(int offset) override;
	virtual unsigned int pos() override;

private:
	std::ifstream file;
	unsigned int offset;
	unsigned int size;
};

Result<std::unique_ptr<SoLoudBundleFile>>
openFileFromBundle(AssetsBundle& bundle, std::string name) noexcept;

} // namespace Growl
