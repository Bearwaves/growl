#pragma once

#include <string>
namespace Growl {

class SFX {
public:
	SFX(std::string name)
		: name{name} {}
	virtual ~SFX() = default;
	// SFX is move-only
	SFX(const SFX&) = delete;
	SFX& operator=(const SFX&) = delete;
	SFX(SFX&&) = default;
	SFX& operator=(SFX&&) = default;

private:
	std::string name;
};

bool isValidAudio(std::string path);

} // namespace Growl
