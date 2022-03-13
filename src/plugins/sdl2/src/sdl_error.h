#pragma once

#include <growl/util/error.h>
namespace Growl {

class SDL2Error : public BaseError {
public:
	explicit SDL2Error(std::string message)
		: message_str{message} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
