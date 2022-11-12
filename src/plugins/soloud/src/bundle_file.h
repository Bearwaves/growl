#pragma once

#include "growl/core/assets/file.h"
#include "growl/core/error.h"
#include "soloud_file.h"
#include <memory>

namespace Growl {

class AssetsBundle;
class SystemAPI;

class SoLoudBundleFile : public SoLoud::File {
public:
	explicit SoLoudBundleFile(std::unique_ptr<Growl::File> file)
		: file{std::move(file)} {}
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
	std::unique_ptr<Growl::File> file;
};

Result<std::unique_ptr<SoLoudBundleFile>> openFileFromBundle(
	SystemAPI& system, AssetsBundle& bundle, std::string name) noexcept;

} // namespace Growl
