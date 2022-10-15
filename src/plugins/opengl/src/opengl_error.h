#pragma once

#include "growl/core/error.h"
namespace Growl {

class OpenGLError : public BaseError {
public:
	explicit OpenGLError(std::string message)
		: message_str{message} {}

	std::string message() override {
		return message_str;
	}

private:
	std::string message_str;
};

} // namespace Growl
