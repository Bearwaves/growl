#pragma once

#include <string>
namespace Growl {

class Clip {
public:
	Clip(std::string name)
		: name{name} {}
	virtual ~Clip() = default;
	// SFX is move-only
	Clip(const Clip&) = delete;
	Clip& operator=(const Clip&) = delete;
	Clip(Clip&&) = default;
	Clip& operator=(Clip&&) = default;

private:
	std::string name;
};

bool isValidAudio(std::string path);

} // namespace Growl
